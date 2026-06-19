#pragma once
#include <memory>
#include <vector>
#include "communication.h"
#include "card.h"
#include "localization.h"
#include "card_color.h"
#include <thread>
#include <string>
#include <stop_token>
#include <optional>



enum class MenuState {
	GAMEPLAY,
	PAUSED,
	GLOBAL
};

class GameUserInterface
{
private:
	std::shared_ptr<CommunicationHub<BackendToFrontendEvent, FrontendToBackendResponse>> m_hub;
	std::vector<PlayerState> m_player_states;
	std::vector<LocalPlayerCards> m_local_players_cards;
	int m_current_player_in_round;
	int m_cards_to_draw;
	int m_cards_placed;
	std::string m_event;
	Card m_last_card_placed;
	std::string m_last_displayed_ui;
	bool m_paused;
	std::string m_last_frame_for_pause;
	std::jthread m_ui_loop_thread;
	MenuState m_menu_state;
	std::stop_token m_stop;
	bool m_game_over;

	bool ui_pause_menu(MenuState menu_state);
	std::optional<char> wait_for_key(MenuState state) const;
	void ui_loop(std::stop_token stop, MenuState menu_state);
	void print_to_screen(MenuState state);
	void clear();
	std::string get_string_from_localization(LocalizationKey key);
	std::vector<Card> get_selectable_cards(int player_id, std::vector<int> selectable_cards_ids);
	std::string get_card_description_in_string(Card card);
	std::string get_player_name_from_id(int player_id);
	void update_player_state(std::vector<UpdatePlayerState> ups);
	int ui_choose_card(int player_id, std::vector<Card> cards, MenuState menu_state);
	CardColor ui_choose_color(int player_id, MenuState menu_state);
	void ui_game_over(int player_id, MenuState menu_state);
	void update_event(Events event);

public:
	GameUserInterface(std::shared_ptr<CommunicationHub<BackendToFrontendEvent, FrontendToBackendResponse>> input_hub);
	void start_ui_loop();
};

