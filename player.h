#pragma once
#include "card.h"
#include <vector>
#include "control_player.h"
#include <string>
#include <memory>


enum PlayerType
{
	LOCAL = 0,
	ROBOT = 1,
};


class Player
{
private:
	int m_id;
	std::string m_player_name;
	std::vector<Card> m_cards;
	PlayerType m_type;
	std::unique_ptr<ControlPlayer> m_controller;

public:
	Player(int player_id, std::string input_player_name, PlayerType input_type, std::unique_ptr<ControlPlayer> controller, std::vector<Card> cards);
	std::string get_player_name() const;
	std::vector<Card> get_cards() const;
	void add_card(std::vector<Card> added_card);
	void remove_card(int index);
	PlayerType get_player_type() const;
	int get_player_id() const;
	ControlPlayer* get_controller();
	Card get_card_by_id(int card_id);
	
};
