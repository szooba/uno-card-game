#include "card_type.h"
#include "localization.h"

CardType::CardType()
{
    m_type = EnumCardType::WILD;
}

CardType::CardType(EnumCardType type)
{
	m_type = type;
}

LocalizationKey CardType::get_type_to_localizationkey() const
{
    LocalizationKey type_key;
    switch (m_type) {
    case EnumCardType::NUMBER:
        type_key = LocalizationKey::CardTypeNumber;
        break;
    case EnumCardType::SKIP:
        type_key = LocalizationKey::CardTypeSkip;
        break;
    case EnumCardType::REVERSE:
        type_key = LocalizationKey::CardTypeReverse;
        break;
    case EnumCardType::DRAW:
        type_key = LocalizationKey::CardTypeDraw;
        break;
    case EnumCardType::WILD:
        type_key = LocalizationKey::CardTypeWild;
        break;
    }
    return type_key;
}

EnumCardType CardType::get_enum_type() const
{
	return m_type;
}

void CardType::edit_type(EnumCardType type)
{
	m_type = type;
}
