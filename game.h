#pragma once
#include <vector>
#include "player.h"
#include "profile.h"
#include "communication.h"
#include <thread>
#include <stop_token>
#include <memory>
#include "card.h"
class Game
{
private:
	std::vector<Player> m_players;
	int m_turn_mode;
	std::shared_ptr<CommunicationHub<BackendToFrontendEvent, FrontendToBackendResponse>> m_hub;
	int m_card_ids;
	Card m_last_placed_card;
	int m_current_player_turn;
	int m_cards_placed;
	int m_in_progress_draw_count;
	std::shared_ptr<std::jthread> m_game_loop_thread;
	int m_starting_player_id;
	Events m_current_event;
	int m_delay_between_events;

	void game_loop(std::stop_token stop);
	void prepare_players(std::vector<Profile> local_players, int robot_players, int starting_deck);
	std::vector<Card> generate_deck(int starting_deck_count);
	std::vector<PlayerState> get_player_states();
	int generate_card_id();
	std::vector<LocalPlayerCards> get_local_players_cards();
	std::vector<UpdatePlayerState> get_updated_players_states();
	std::vector<int> collect_chooseable_cards();
	void process_card(int selected_card);
	void broadcast_progress(BackendToFrontendEvent btfe);
	bool can_it_be_placed(Card card);
	int calc_turn(int add_step);
	void modify_turn(int add_step);
	void reverse_turn(int turn);
	bool check_for_game_over();
	void draw_card(int how_many);
	int apply_in_progress_draw();

public:
	Game(std::vector<Profile> local_players, int robot_players, int starting_deck, int delay_between_events, std::shared_ptr<CommunicationHub<BackendToFrontendEvent, FrontendToBackendResponse>> hub);
	void create_game_loop_thread();
	std::shared_ptr<std::jthread> get_game_thread();
	void shutdown_game();
};



