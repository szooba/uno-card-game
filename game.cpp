#include "game.h"
#include "localization.h"
#include "control_player_local.h"
#include "control_player_robot.h"
#include <vector>

Game::Game(std::vector<Profile> local_players,
	int robot_players,
	int starting_deck,
	int delay_between_events,
	std::shared_ptr<CommunicationHub<BackendToFrontendEvent, FrontendToBackendResponse>> hub)
	: m_last_placed_card(generate_card_id()),
	m_hub(hub),
	m_delay_between_events(delay_between_events),
	m_current_event(EventText(LocalizationKey::GameStarted))
{
	prepare_players(local_players, robot_players, starting_deck);
}

void Game::create_game_loop_thread() {
	m_game_loop_thread = std::make_shared<std::jthread>([this](std::stop_token stop) {
		this->game_loop(stop);
	});
}

std::shared_ptr<std::jthread> Game::get_game_thread() {
	return m_game_loop_thread;
}

void Game::shutdown_game() {
	get_game_thread().get()->request_stop();
	m_hub->close_channels();
}

void Game::game_loop(std::stop_token stop)
{
	m_current_player_turn = -1;
	m_turn_mode = 1;
	broadcast_progress(GameStarted{ get_player_states(), m_last_placed_card });
	while (!stop.stop_requested()) {
		modify_turn(m_turn_mode);
		broadcast_progress(TurnUpdate{ m_current_player_turn, m_cards_placed, m_in_progress_draw_count, m_current_event, m_last_placed_card, get_updated_players_states(), get_local_players_cards() });
		std::vector<int> chooseable_cards = collect_chooseable_cards();
		if (chooseable_cards.empty()) {
			draw_card(1 + apply_in_progress_draw());
		}
		else {
			std::optional<CardChoiceResponse> response = m_players[m_current_player_turn].get_controller()->request_card_choice(RequestCardChoice{ m_current_player_turn ,collect_chooseable_cards() });
			if (response.has_value()) {
				process_card(response.value().selected_indices);
			}
		}
		broadcast_progress(TurnUpdate{ m_current_player_turn, m_cards_placed, m_in_progress_draw_count, m_current_event, m_last_placed_card, get_updated_players_states(), get_local_players_cards() });
		if (check_for_game_over()) {
			return;
		}
	}
}



void Game::prepare_players(std::vector<Profile> local_players, int robot_players, int starting_deck)
{
	int player_id = 0;
	for (const auto& profile : local_players) {
		m_players.push_back(Player(player_id, profile.get_name(),PlayerType::LOCAL, std::make_unique<ControlPlayerLocal>(m_hub), generate_deck(starting_deck)));
		player_id++;
	}

	for (size_t i = 0; i < robot_players; ++i) {
		m_players.push_back(Player(player_id, "Robot " + std::to_string(player_id - int(local_players.size()) + 1), PlayerType::ROBOT, std::make_unique<ControlPlayerRobot>(), generate_deck(starting_deck)));
		player_id++;
	}

}

std::vector<Card> Game::generate_deck(int starting_deck_count)
{
	std::vector<Card> cards;
	for (size_t i = 0; i < starting_deck_count; ++i) {
		cards.push_back(Card(generate_card_id()));
	}
	return cards;
}

std::vector<PlayerState> Game::get_player_states()
{
	std::vector<PlayerState> player_states;
	for (const auto& player : m_players) {
		player_states.push_back(PlayerState{ player.get_player_id(), player.get_player_name(), int(player.get_cards().size())});
	}
	return player_states;
}

int Game::generate_card_id()
{
	m_card_ids++;
	return m_card_ids;
}

std::vector<LocalPlayerCards> Game::get_local_players_cards()
{
	std::vector<LocalPlayerCards> local_player_cards;
	for (const auto& player : m_players) {
		if (player.get_player_type() == PlayerType::LOCAL) {
			local_player_cards.push_back(LocalPlayerCards{ player.get_player_id(),player.get_cards() });
		}
	}
	return local_player_cards;
}

std::vector<UpdatePlayerState> Game::get_updated_players_states()
{
	std::vector<UpdatePlayerState> updated_player_states;
	for (const auto& player : m_players) {
		updated_player_states.push_back(UpdatePlayerState{ player.get_player_id(), int(player.get_cards().size()) });
	}
	return updated_player_states;
}

std::vector<int> Game::collect_chooseable_cards()
{
	std::vector<int> collected_cards;
	for (const auto& card : m_players[m_current_player_turn].get_cards()) {
		if (can_it_be_placed(card)) {
			collected_cards.push_back(card.get_card_id());
		}
	}
	return collected_cards;
}


void Game::process_card(int selected_card)
{
	m_last_placed_card = m_players[m_current_player_turn].get_card_by_id(selected_card);
	std::optional<CardColorChoiceResponse> cccr;
	m_players[m_current_player_turn].remove_card(selected_card);
	m_cards_placed += 1;

	switch (m_last_placed_card.get_type().get_enum_type()) {
	case EnumCardType::SKIP:
		draw_card(apply_in_progress_draw());
		m_current_event = EventSkip{ m_players[m_current_player_turn].get_player_id(), m_players[calc_turn(m_turn_mode)].get_player_id()};
		modify_turn(m_turn_mode);
		break;
	case EnumCardType::REVERSE:
		draw_card(apply_in_progress_draw());
		m_current_event = EventReverse{ m_players[m_current_player_turn].get_player_id() };
		reverse_turn(-1);
		break;
	case EnumCardType::DRAW:
		m_in_progress_draw_count += m_last_placed_card.get_number();
		if (m_last_placed_card.get_color().get_enum_color() == EnumCardColor::BLACK) {
			cccr = m_players[m_current_player_turn].get_controller()->request_color_choice(RequestColorChoice{ m_current_player_turn });
			if (cccr.has_value()) {
				m_last_placed_card.edit_card(m_last_placed_card.get_number(), cccr.value().color_code);
			}
		}
		break;
	case EnumCardType::WILD:
		draw_card(apply_in_progress_draw());
		cccr = m_players[m_current_player_turn].get_controller()->request_color_choice(RequestColorChoice{m_current_player_turn});
		if (cccr.has_value()) {
			m_current_event = EventColorChoosen{ m_players[m_current_player_turn].get_player_id(), cccr.value().color_code };
			m_last_placed_card.edit_card(m_last_placed_card.get_number(), cccr.value().color_code);
		}
		break;
	case EnumCardType::NUMBER:
		draw_card(apply_in_progress_draw());
	}
}

void Game::broadcast_progress(BackendToFrontendEvent btfe)
{
	m_hub->send_to_frontend(btfe);
	std::this_thread::sleep_for(std::chrono::seconds(m_delay_between_events));
}

bool Game::can_it_be_placed(Card card) {
	bool accepted_number = false, same_type = false, same_color = false, both_numbers = false, bigger_num = false;

	if (card.get_color().get_enum_color() == m_last_placed_card.get_color().get_enum_color() || card.get_color().get_enum_color() == EnumCardColor::BLACK || m_last_placed_card.get_color().get_enum_color() == EnumCardColor::BLACK) {
		same_color = true;
	}

	if (card.get_type().get_enum_type() == EnumCardType::NUMBER && m_last_placed_card.get_type().get_enum_type() == EnumCardType::NUMBER) {
		both_numbers = true;
	}

	if (card.get_type().get_enum_type() == m_last_placed_card.get_type().get_enum_type() && both_numbers == false) {
		same_type = true;
	}

	if (card.get_number() == m_last_placed_card.get_number() && (card.get_number() != 0 || m_last_placed_card.get_number() != 0) && both_numbers) {
		accepted_number = true;
	}

	if (same_color == true) {
		return true;
	}
	else if (both_numbers && accepted_number) {
		return true;
	}
	else if (same_type) {
		return true;
	}
	return false;
}

int Game::calc_turn(int add_step)
{
	int i = m_current_player_turn;
	if ((m_current_player_turn + add_step) > int(m_players.size()) - 1) {
		i = (m_current_player_turn + add_step) - int(m_players.size());
	}
	else if ((m_current_player_turn + add_step) < 0) {
		i = int(m_players.size()) + add_step;
	}
	else {
		i += add_step;
	}
	return i;
}

void Game::modify_turn(int add_step)
{
	m_current_player_turn = calc_turn(add_step);
}

void Game::reverse_turn(int turn)
{
	m_turn_mode = m_turn_mode * turn;
	if (turn == 1) {
		m_starting_player_id = int(m_players.size());
	}
	else {
		m_starting_player_id = 0;
	}
}

bool Game::check_for_game_over()
{
	for (size_t i = 0; i < int(m_players.size()); ++i) {
		if (m_players[i].get_cards().empty()) {
			broadcast_progress(GameOver{ m_players[i].get_player_id()});
			return true;
		}
	}
	return false;
}

void Game::draw_card(int how_many)
{
	if (how_many == 0) {
		return;
	}
	std::vector<Card> cards_to_add;
	for (size_t i = 0; i < how_many; ++i) {
		cards_to_add.push_back(Card(generate_card_id()));
	}
	m_players[m_current_player_turn].add_card(cards_to_add);
	m_current_event = EventDraw{ m_current_player_turn, how_many };
	
}

int Game::apply_in_progress_draw()
{
	if (m_in_progress_draw_count == 0) {
		return 0;
	}
	int i = m_in_progress_draw_count;
	m_in_progress_draw_count = 0;
	return i;
}


