#include "game_user_interface.h"
#include "localization.h"
#include "card.h"
#include "communication.h"
#include "card_type.h"
#include "card_color.h"
#include <conio.h>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <iostream>
#include <format>
#include <thread>
#include <stop_token>
#include <memory>
#include <cstdlib>
#include <algorithm>
#include <chrono>

GameUserInterface::GameUserInterface(std::shared_ptr<CommunicationHub<BackendToFrontendEvent, FrontendToBackendResponse>> input_hub)
	: m_hub(input_hub),
	m_last_card_placed(0),
	m_paused(false),
	m_menu_state(MenuState::GAMEPLAY),
	m_cards_placed(0),
	m_cards_to_draw(0),
	m_current_player_in_round(0),
	m_game_over(false)
{

}

void GameUserInterface::start_ui_loop() {
	m_ui_loop_thread = std::jthread([this](std::stop_token stop) {
		this->ui_loop(stop, MenuState::GAMEPLAY);
	});
	while (!m_game_over) {
		std::optional<char> c = wait_for_key(MenuState::GLOBAL);
		if (c.has_value()) {
			if (c.value() == 27) {
				m_menu_state = MenuState::PAUSED;
				bool exit = ui_pause_menu(MenuState::PAUSED);
				clear();
				std::cout << m_last_frame_for_pause;
				if (exit) {
					m_ui_loop_thread.request_stop();
					m_menu_state = MenuState::GAMEPLAY;
					return;
				}
				m_menu_state = MenuState::GAMEPLAY;
			}
		}
	std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}

bool GameUserInterface::ui_pause_menu(MenuState menu_state) {
	std::vector<LocalizationKey> menu = {LocalizationKey::GameMenuPauseBackToGame, LocalizationKey::GameMenuPauseQuit};

	size_t pos = 0;
	size_t last_pos = 1;

	while (true) {
		if (menu_state == m_menu_state) {
			std::string frame = "";
			for (size_t i = 0; i < menu.size(); i++) {
				if (pos == i) {
					frame += "- ";
				}
				else {
					frame += "  ";
				}
				frame += get_string_from_localization(menu[i]) + "\n";
			}

			if (last_pos != pos) {
				clear();
				std::cout << frame;
				last_pos = pos;
			}
		}

		std::optional<char> c = wait_for_key(menu_state);
		if (c.has_value()) {
			switch ((int)c.value()) {
			case 80:
				if (pos == menu.size() - 1) {
					pos = 0;
				}
				else {
					pos++;
				}
				break;

			case 72:
				if (pos == 0) {
					pos = menu.size() - 1;
				}
				else {
					pos--;
				}
				break;

			case 13:
				switch (pos) {
				case 0:
					return false;

				case 1:
					return true;

				default:
					return false;
					break;
				}
				break;
			}
		}
	std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}

std::optional<char> GameUserInterface::wait_for_key(MenuState state) const{
	char c = ' ';
	if (_kbhit() && state == MenuState::GLOBAL) {
		c = _getch();
		_ungetch(c);
		return c;
	}
	
	if (_kbhit() && m_menu_state == state) {
		c = _getch();
		return c;
	}
	return std::nullopt;
}

void GameUserInterface::ui_loop(std::stop_token stop, MenuState menu_state)
{
	m_stop = stop;
	while (!stop.stop_requested()) {
		auto msg = m_hub->listen_to_backend();
		if (msg.has_value()) {
			if (const GameStarted* received_message = std::get_if<GameStarted>(&msg.value())) {
				m_player_states = received_message->player_states;
				m_last_card_placed = received_message->starting_card;
			}
			else if (const TurnUpdate* received_message = std::get_if<TurnUpdate>(&msg.value())) {
				m_current_player_in_round = received_message->current_turn_index;
				m_cards_placed = received_message->cards_placed;
				m_last_card_placed = received_message->last_card_placed;
				m_local_players_cards = received_message->updated_local_player_cards;
				m_cards_to_draw = received_message->in_progress_draw_card_count;
				update_player_state(received_message->update_card_counts);
				update_event(received_message->event);
			}
			else if (const RequestCardChoice* received_message = std::get_if<RequestCardChoice>(&msg.value())) {
				int gotten_player_id = received_message->player_id;
				std::vector<int> player_cards_id = received_message->player_cards_id;
				int choosen_card_id = ui_choose_card(gotten_player_id, get_selectable_cards(gotten_player_id, player_cards_id), menu_state);
				m_hub->send_to_backend(CardChoiceResponse{ choosen_card_id });
			}
			else if (const RequestColorChoice* received_message = std::get_if<RequestColorChoice>(&msg.value())) {
				CardColor cc = ui_choose_color(received_message->player_id, menu_state);
				m_hub->send_to_backend(CardColorChoiceResponse{ cc });
			}
			else if (const GameOver* received_message = std::get_if<GameOver>(&msg.value())) {
				ui_game_over(received_message->winner_player_id, menu_state);
				m_game_over = true;
				return;
			}
		}
		print_to_screen(menu_state);
	}
}

void GameUserInterface::print_to_screen(MenuState state)
{
	std::string s;
	for (int i = 0; i < m_player_states.size(); i++) {
		if (i == m_current_player_in_round) {
			s += "|" + m_player_states[i].player_name + "|";
		}
		else {
			s += m_player_states[i].player_name;
		}
		if (m_cards_to_draw > 0 && i == m_current_player_in_round) {
			s += ": " + std::to_string(m_player_states[i].player_card_count) + "+" + std::to_string(m_cards_to_draw) + ", ";
		}
		else {
			s += ": " + std::to_string(m_player_states[i].player_card_count) + ", ";
		}
	}
	s += "\n\n";

	s += std::vformat(Localization::get_instance().get_text_from_string(LocalizationKey::GameCardsPlaced), std::make_format_args(m_cards_placed)) + "\n\n";

	std::string temp_str = get_string_from_localization(LocalizationKey::GameEvent);

	s += std::vformat(temp_str, std::make_format_args(m_event)) + "\n\n";

	temp_str = get_string_from_localization(LocalizationKey::GameLastCard);

	std::string card_desc = get_card_description_in_string(m_last_card_placed);

	s += std::vformat(temp_str, std::make_format_args(card_desc)) + "\n\n";

	for (const auto& player : m_local_players_cards) {
		s += get_player_name_from_id(player.player_id) + "\n";
		
		for (const auto& card : player.cards) {
			s += get_card_description_in_string(card) + "\n";
		}
		s += "\n";
	}

	if (m_menu_state == state) {
		clear();
		std::cout << s;
		
	}
	else {
		m_last_frame_for_pause = s;
	}
	m_last_displayed_ui = s;
}

void GameUserInterface::clear()
{
	system("cls");
}

std::string GameUserInterface::get_string_from_localization(LocalizationKey key)
{
	return Localization::get_instance().get_text_from_string(key);
}

std::vector<Card> GameUserInterface::get_selectable_cards(int player_id, std::vector<int> selectable_cards_ids) {
	std::vector<Card> selectable_cards;
	for (const auto& player : m_local_players_cards) {
		if (player.player_id != player_id) {
			continue;
		}
		
		for (const auto& card : player.cards) {
			auto it = std::find(selectable_cards_ids.begin(), selectable_cards_ids.end(), card.get_card_id());
			if (it != selectable_cards_ids.end()) {
				selectable_cards.push_back(card);
			}
		}
		break;
	}
	return selectable_cards;
}

std::string GameUserInterface::get_card_description_in_string(Card card)
{
	std::string description;
	switch (card.get_type().get_enum_type()) {
	case EnumCardType::NUMBER:
		description = std::to_string(card.get_number()) + " " + get_string_from_localization(card.get_color().get_color_to_localizationkey());
		break;
	case EnumCardType::SKIP:
		description = get_string_from_localization(card.get_type().get_type_to_localizationkey()) + " " + get_string_from_localization(card.get_color().get_color_to_localizationkey());
		break;
	case EnumCardType::REVERSE:
		description = get_string_from_localization(card.get_type().get_type_to_localizationkey()) + " " + get_string_from_localization(card.get_color().get_color_to_localizationkey());
		break;
	case EnumCardType::DRAW:
		description = get_string_from_localization(card.get_type().get_type_to_localizationkey()) + " " + std::to_string(card.get_number()) + " " + get_string_from_localization(card.get_color().get_color_to_localizationkey());
		break;
	case EnumCardType::WILD:
		description = get_string_from_localization(card.get_type().get_type_to_localizationkey());
		break;
	default:
		description = "UNKNOWN";
	}
	return description;
}

std::string GameUserInterface::get_player_name_from_id(int player_id)
{
	for (const auto& player : m_player_states) {
		if (player.player_id == player_id) {
			return player.player_name;
		}
	}
	return "";
}

void GameUserInterface::update_player_state(std::vector<UpdatePlayerState> ups)
{
	for (const auto& updated_player_state : ups) {
		
		for (auto& player_state : m_player_states) {
			
			if (updated_player_state.player_id == player_state.player_id) {
				player_state.player_card_count = updated_player_state.player_card_count;
			}
		}
	}
}

int GameUserInterface::ui_choose_card(int player_id, std::vector<Card> cards, MenuState menu_state)
{
	size_t pos = 0;
	std::string last_frame = "";

	while (!m_stop.stop_requested()) {
		if (menu_state == m_menu_state) {
			std::cout << last_frame;
			std::string s = m_last_displayed_ui;
			std::string frame = "";
			frame += s + get_string_from_localization(LocalizationKey::GamePlayerCards) + "\n";

			for (size_t i = 0; i < cards.size(); i++) {
				if (pos == i) {
					frame += "- ";
				}
				else {
					frame += "  ";
				}
				frame += get_card_description_in_string(cards[i]) + "\n";
			}
			if (m_last_frame_for_pause != frame || m_last_frame_for_pause.empty()) {
				clear();
				std::cout << frame;
				m_last_frame_for_pause = frame;
			}
		}

		std::optional<char> c = wait_for_key(menu_state);

		if (c.has_value()) {
			switch ((int)c.value()) {
			case 80:
				if (pos == cards.size() - 1) {
					pos = 0;
				}
				else {
					pos++;
				}
				break;

			case 72:
				if (pos == 0) {
					pos = cards.size() - 1;
				}
				else {
					pos--;
				}
				break;

			case 13:
				return cards[pos].get_card_id();
			}
		}
		else {}
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
	return cards[0].get_card_id();
}

CardColor GameUserInterface::ui_choose_color(int player_id, MenuState menu_state)
{
	size_t pos = 0;

	std::vector<CardColor> card_colors;
	for (size_t i = 0; i < int(EnumCardColor::COUNT) - 1; ++i) {
		card_colors.push_back(CardColor(EnumCardColor(i)));
	}

	while (!m_stop.stop_requested()) {
		if (menu_state == m_menu_state) {
			std::string s = m_last_displayed_ui;
			std::string frame = "";

			frame += s + get_string_from_localization(LocalizationKey::GamePlayerCards) + "\n";

			for (int i = 0; i < card_colors.size(); i++) {
				if (pos == i) {
					frame += "- ";
				}
				else {
					frame += "  ";
				}
				frame += get_string_from_localization(card_colors[i].get_color_to_localizationkey()) + "\n";
			}

			if (m_last_frame_for_pause != frame || m_last_frame_for_pause.empty()) {
				clear();
				std::cout << frame;
				m_last_frame_for_pause = frame;
			}
		}
		std::optional<char> c = wait_for_key(menu_state);

		if (c.has_value()) {
			switch ((int)c.value()) {
			case 80:
				if (pos == card_colors.size() - 1) {
					pos = 0;
				}
				else {
					pos++;
				}
				break;

			case 72:
				if (pos == 0) {
					pos = card_colors.size() - 1;
				}
				else {
					pos--;
				}
				break;

			case 13:
				return card_colors[pos].get_enum_color();
			}
		}
		else {}
	}
	return card_colors[0].get_enum_color();
}

void GameUserInterface::ui_game_over(int player_id, MenuState menu_state)
{
	std::string event = get_string_from_localization(LocalizationKey::GamePlayerMatchWon);
	std::string temp_str = get_player_name_from_id(player_id);

	std::string frame = m_last_displayed_ui;
	frame += std::vformat(event, std::make_format_args(temp_str)) + "\n";
	frame += get_string_from_localization(LocalizationKey::GameContinue) + "\n\n";

	while (!m_stop.stop_requested()) {
		if (menu_state == m_menu_state) {

			if (m_last_frame_for_pause != frame || m_last_frame_for_pause.empty()) {
				clear();
				std::cout << frame;
				m_last_frame_for_pause = frame;
			}
		}

		std::optional<char> c = wait_for_key(menu_state);

		if (c.has_value()) {
			return;
		}
		else {
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
	return;
}

void GameUserInterface::update_event(Events event)
{
	if (const EventText* received_message = std::get_if<EventText>(&event)) {
		m_event = get_string_from_localization(received_message->key);
	}
	if (const EventSkip* received_message = std::get_if<EventSkip>(&event)) {
		std::string player1 = get_player_name_from_id(received_message->player_id_1);
		std::string player2 = get_player_name_from_id(received_message->player_id_2);
		std::string key_string = get_string_from_localization(LocalizationKey::GamePlayerSkip);
		m_event = std::vformat(key_string, std::make_format_args(player1, player2));
	}
	if (const EventColorChoosen* received_message = std::get_if<EventColorChoosen>(&event)) {
		std::string player = get_player_name_from_id(received_message->player_id);
		std::string color = get_string_from_localization(received_message->color.get_color_to_localizationkey());
		std::string key_string = get_string_from_localization(LocalizationKey::GamePlayerColorChoosed);
		m_event = std::vformat(key_string, std::make_format_args(player, color));
	}
	if (const EventDraw* received_message = std::get_if<EventDraw>(&event)) {
		std::string player = get_player_name_from_id(received_message->player_id);
		std::string count = std::to_string(received_message->how_many);
		std::string key_string = get_string_from_localization(LocalizationKey::GamePlayerDrawCard);
		m_event = std::vformat(key_string, std::make_format_args(player, count));
	}
	if (const EventReverse* received_message = std::get_if<EventReverse>(&event)) {
		std::string player = get_player_name_from_id(received_message->player_id);
		std::string key_string = get_string_from_localization(LocalizationKey::GamePlayerReverse);
		m_event = std::vformat(key_string, std::make_format_args(player));
	}
}






