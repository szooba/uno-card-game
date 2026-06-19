#pragma once
#include "localization.h"

enum class EnumCardType {
	NUMBER,
	SKIP,
	REVERSE,
	DRAW,
	WILD,

	COUNT

};

class CardType
{
private:
	EnumCardType m_type;
public:
	CardType();
	CardType(EnumCardType type);
	LocalizationKey get_type_to_localizationkey() const;
	EnumCardType get_enum_type() const;
	void edit_type(EnumCardType type);
};

