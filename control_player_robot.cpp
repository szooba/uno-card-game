#include "control_player_robot.h"
#include "card.h"
#include "random.h"

ControlPlayerRobot::ControlPlayerRobot()
{
}

std::optional<CardChoiceResponse> ControlPlayerRobot::request_card_choice(RequestCardChoice rcc)
{
    return CardChoiceResponse{ rcc.player_cards_id[Random::get_int(0, int(rcc.player_cards_id.size()) - 1)]};
}

std::optional<CardColorChoiceResponse> ControlPlayerRobot::request_color_choice(RequestColorChoice rcc)
{
    return CardColorChoiceResponse{ EnumCardColor(Random::get_int(0, (int(EnumCardColor::COUNT) - 2))) };
}
