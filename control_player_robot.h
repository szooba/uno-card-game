#pragma once
#include "control_player.h"
#include "communication.h"
class ControlPlayerRobot : public ControlPlayer
{
private:

public:
	ControlPlayerRobot();

	std::optional<CardChoiceResponse> request_card_choice(RequestCardChoice rcc) override;
	std::optional<CardColorChoiceResponse> request_color_choice(RequestColorChoice rcc) override;
};

