#pragma once
#include "profile.h"
#include <vector>
#include "localization.h"
#include <string>
#include <optional>

struct Settings {
    int m_local_players;
    int m_robot_players;
    int m_starting_deck;
    int m_delay_between_events;
};

class MainMenu
{
private:
    Profile m_profile = Profile("!");
    std::vector<int> m_options_value;
    void single(Settings set) const;
    std::optional<Settings> set_game_settings();
    std::string get_string_from_localization(LocalizationKey key);
public:
    void main();
    void language_selection();
    void create_profile_menu();
};

