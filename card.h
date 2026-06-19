#pragma once
#include "card_color.h"
#include "card_type.h"

class Card
{
private:
	int m_card_id;
	CardColor m_color;
	int m_num;
	CardType m_type;

	CardColor random_color();
	CardType random_type();
	int random_number(int first, int last);

public:
	Card(int card_id);
	Card(CardColor input_color, int input_num, CardType input_type);
	CardColor get_color() const;
	int get_number() const;
	void edit_card(int input_num, CardColor input_col);
	void edit_card(CardColor col);
	CardType get_type() const;
	int get_card_id() const;
};

