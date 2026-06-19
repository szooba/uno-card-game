#pragma once
#include "communication.h"
class ControlPlayer
{
public:
	virtual ~ControlPlayer() = default;
	virtual std::optional<CardChoiceResponse> request_card_choice(RequestCardChoice rcc);
	virtual std::optional<CardColorChoiceResponse> request_color_choice(RequestColorChoice rcc);
};

