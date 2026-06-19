#include "card.h"
#include "control_player_robot.h"
#include "player.h"
#include "random.h"


Card::Card(int card_id)
{
    
    m_card_id = card_id;
    m_type = CardType(EnumCardType(Random::random_type()));

    int two_or_four[3] = { 2,2,4 };
    switch (m_type.get_enum_type()) {
    case EnumCardType::SKIP:
        m_color = random_color();
        break;
    case EnumCardType::REVERSE:
        m_color = random_color();
        m_num = 0;
        break;
    case EnumCardType::DRAW:
        m_num = two_or_four[rand() % 2];
        if (m_num == 4) {
            m_color = CardColor(EnumCardColor::BLACK);
        }
        else {
            m_color = random_color();
        }
        break;
    case EnumCardType::WILD:
        m_color = EnumCardColor::BLACK;
        m_num = 0;
        break;
    default:
        m_color = random_color();
        m_num = random_number(2,9);
    }
}

Card::Card(CardColor input_color, int input_num, CardType input_type)
{
    m_type = input_type;
    m_num = input_num;
    m_color = input_color;
    m_card_id = 0;
}

CardColor Card::get_color() const
{
	return m_color;
}


int Card::get_number() const
{
	return m_num;
}

void Card::edit_card(int input_num, CardColor input_col)
{
	m_num = input_num;
	m_color = input_col;
}

void Card::edit_card(CardColor col)
{
	m_color = col;
}

CardType Card::get_type() const
{
	return m_type;
}

CardColor Card::random_color()
{
    return CardColor(EnumCardColor(Random::get_int(0, (int(EnumCardColor::COUNT) - 2))));
}

CardType Card::random_type()
{
    return CardType(EnumCardType(Random::get_int(0, (int(EnumCardType::COUNT)-1))));
}

int Card::random_number(int first, int last)
{
    return Random::get_int(first, last);
}

int Card::get_card_id() const
{
    return m_card_id;
}
