#include "card_color.h"
#include "localization.h"

CardColor::CardColor()
{
    m_color = EnumCardColor::BLACK;
}

CardColor::CardColor(EnumCardColor color)
{
    m_color = color;
}

LocalizationKey CardColor::get_color_to_localizationkey() const
{
    LocalizationKey color_key;
    switch (m_color) {
    case EnumCardColor::RED:
        color_key = LocalizationKey::CardColorRed;
        break;
    case EnumCardColor::BLUE:
        color_key = LocalizationKey::CardColorBlue;
        break;
    case EnumCardColor::GREEN:
        color_key = LocalizationKey::CardColorGreen;
        break;
    case EnumCardColor::YELLOW:
        color_key = LocalizationKey::CardColorYellow;
        break;
    case EnumCardColor::BLACK:
        color_key = LocalizationKey::CardColorBlack;
        break;
    }

    return color_key;
}

EnumCardColor CardColor::get_enum_color() const
{
	return m_color;
}

void CardColor::edit_color(EnumCardColor col)
{
    m_color = col;
}
