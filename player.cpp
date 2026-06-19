#include "player.h"

Player::Player(int player_id, std::string input_player_name, PlayerType input_type, std::unique_ptr<ControlPlayer> controller, std::vector<Card> cards)
{
	m_id = player_id;
	m_player_name = input_player_name;
	m_type = input_type;
	m_cards = cards;
	m_controller = std::move(controller);
}

std::string Player::get_player_name() const
{
	return m_player_name;
}

std::vector<Card> Player::get_cards() const
{
	return m_cards;
}

void Player::add_card(std::vector<Card> added_cards)
{
	for (const auto& card : added_cards) {
		m_cards.push_back(card);
	}
}

void Player::remove_card(int index)
{
	for (size_t i = 0; i < m_cards.size(); ++i) {
		if (m_cards[i].get_card_id() == index) {
			m_cards.erase(m_cards.begin() + i);
		}
	}
}

int Player::get_player_id() const
{
	return m_id;
}

ControlPlayer* Player::get_controller()
{
	return m_controller.get();
}

Card Player::get_card_by_id(int card_id)
{
	for (const auto& card : m_cards) {
		if (card.get_card_id() == card_id) {
			return card;
		}
	}
	return Card(0);
}

PlayerType Player::get_player_type() const
{
	return m_type;
}