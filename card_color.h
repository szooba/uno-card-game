#pragma once
#include "localization.h"

enum class EnumCardColor {
	RED,
	BLUE,
	GREEN,
	YELLOW,
	BLACK,

	COUNT
};

class CardColor
{
private:
	EnumCardColor m_color;
public:
	CardColor();
	CardColor(EnumCardColor color);
	LocalizationKey get_color_to_localizationkey() const;
	EnumCardColor get_enum_color() const;
	void edit_color(EnumCardColor col);
};

