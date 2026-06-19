#include "main_menu.h"
#include "localization.h"
#include "communication.h"
#include "game.h"
#include "game_user_interface.h"
#include <vector>
#include <string>
#include <ostream>
#include <optional>
#include <memory>
#include <iostream>
#include <cstdlib>
#include <conio.h>
#include "profile.h"


void MainMenu::main() {
    std::vector<LocalizationKey> menu = { LocalizationKey::MainMenuSingleplayer,LocalizationKey::MainMenuChangeLanguage, LocalizationKey::MainMenuChangeName, LocalizationKey::MainMenuExit };
    size_t pos = 0;
    std::optional <Settings> game_settings;
    m_options_value = { 1,3,7,2 };
    while (true) {
        system("cls");
        std::cout << get_string_from_localization(LocalizationKey::MainMenuPlayer) << m_profile.get_name() << "\n" << std::endl;
        for (size_t i = 0; i < menu.size(); i++) {
            if (pos == i) {
                std::cout << "- ";
            }
            std::cout << get_string_from_localization(menu[i]) << std::endl;
            if (i == menu.size() - 2) {
                std::cout << "\n";
            }
        }
        char c = _getch();
        switch ((int)c) {
        case 80:
            if (pos == menu.size() - 1) {
                pos = 0;
                break;
            }
            pos++;
            break;
        case 72:
            if (pos == 0) {
                pos = menu.size() - 1;
                break;
            }
            pos--;
            break;
        case 13:
            switch (pos) {
            default:
                exit(EXIT_SUCCESS);
            case 0:
                game_settings = set_game_settings();
                if (game_settings.has_value()) {
                    single(game_settings.value());
                }
                else {
                    continue;
                }
                break;
            case 1:
                language_selection();
                break;
            case 2:
                create_profile_menu();
                break;
            }
            break;
        }
    }
}


std::optional<Settings> MainMenu::set_game_settings() {
    std::vector<LocalizationKey> options = { LocalizationKey::SettingsMenuLocalPlayers,LocalizationKey::SettingsMenuRobotPlayers,LocalizationKey::SettingsMenuDeck, LocalizationKey::MainMenuDelayEvents, LocalizationKey::MainMenuBeginGame,LocalizationKey::MainMenuExit };
    std::string names = "";
    size_t pos = 0;
    while (true) {
        system("cls");
        for (int i = 0; i < options.size(); i++) {
            if (pos == i) {
                if (pos <= 3) {
                    std::cout << "<";
                }
                std::cout << "- ";
            }
            std::cout << get_string_from_localization(options[i]);
            if (i <= options.size() - 3) {
                std::cout << " " << m_options_value[i];
            }
            if (pos == i && pos < 4) {
                std::cout << " ->";
            }
            std::cout << "\n";
            if (i == 3) {
                std::cout << "\n";
            }
        }
        char c = _getch();
        switch ((int)c) {
        case 80:
            if (pos == options.size() - 1) {
                pos = 0;
                break;
            }
            pos++;
            break;
        case 72:
            if (pos == 0) {
                pos = options.size() - 1;
                break;
            }
            pos--;
            break;
        case 77:
            if (pos < options.size() - 2) {
                m_options_value[pos] += 1;
            }
            break;
        case 75:
            if (pos < options.size() - 2) {
                if (m_options_value[pos] > 0) {
                    m_options_value[pos] += -1;
                }
            }
            break;
        case 13:
            switch (pos) {
            default:
                return std::nullopt;
            case 3:
                if (m_options_value[0] + m_options_value[1] > 0) {
                    return Settings{ m_options_value[0], m_options_value[1], m_options_value[2], m_options_value[3] };
                }
                else {
                    return std::nullopt;
                }
                break;
            case 4:
                if (m_options_value[0] + m_options_value[1] > 0) {
                    return Settings{ m_options_value[0], m_options_value[1], m_options_value[2], m_options_value[3] };
                }
                else {
                    return std::nullopt;
                }
                break;
            }
            break;
        }
    }
}

std::string MainMenu::get_string_from_localization(LocalizationKey key)
{
    return Localization::get_instance().get_text_from_string(key);
}

void MainMenu::single(Settings set) const {
    auto hub = std::make_shared<CommunicationHub<BackendToFrontendEvent, FrontendToBackendResponse>>();

    std::vector<Profile> profile;
    for (size_t i = 0; i < set.m_local_players; ++i) {
        if (set.m_local_players == 1) {
            profile.push_back(m_profile);
        }
        if (set.m_local_players > 1) {
            profile.push_back(Profile(m_profile.get_name() + " " + std::to_string(i + 1)));
        }
    }
    Game g = Game(profile, set.m_robot_players, set.m_starting_deck, set.m_delay_between_events, hub);
    GameUserInterface hp = GameUserInterface(hub);
    g.create_game_loop_thread();
    hp.start_ui_loop();
    g.shutdown_game();

}

void MainMenu::language_selection() {
    std::vector<std::string> languages = { get_string_from_localization(LocalizationKey::English), get_string_from_localization(LocalizationKey::Hungarian) };
    size_t pos = 0;

    while (true) {
        system("cls");
        std::cout << get_string_from_localization(LocalizationKey::SelectLanguage) << std::endl;

        for (size_t i = 0; i < languages.size(); i++) {
            if (pos == i) {
                std::cout << "- ";
            }
            else {
                std::cout << "  ";
            }
            std::cout << languages[i] << std::endl;
        }

        char c = _getch();
        switch ((int)c) {
        case 80:
            if (pos == languages.size() - 1) pos = 0;
            else pos++;
            break;
        case 72:
            if (pos == 0) pos = languages.size() - 1;
            else pos--;
            break;
        case 13:
            if (pos == 0) {
                Localization::get_instance().init(Language::ENGLISH);
            }
            else if (pos == 1) {
                Localization::get_instance().init(Language::HUNGARIAN);
            }
            return;
        case 27:
            return;
        }
    }
}

void MainMenu::create_profile_menu() {
    std::string profile_name = "";

    system("cls");

    std::cout << get_string_from_localization(LocalizationKey::ProfileMenuCreateGiveName) << ": ";

    std::getline(std::cin, profile_name);
    if (profile_name.empty() || profile_name.find_first_not_of(' ') == std::string::npos) {
        return;
    }
    m_profile = Profile(profile_name);
}
