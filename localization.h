#pragma once
#include <string>
#include <unordered_map>

enum Language {
	ENGLISH,
	HUNGARIAN
};

enum class LocalizationKey {
    SelectLanguage,
    English,
    Hungarian,
    MainMenuSingleplayer,
    MainMenuChangeLanguage,
    MainMenuChangeName,
    MainMenuExit,
    MainMenuPlayer,
    MainMenuProfileChange,
    MainMenuDelayEvents,
    MainMenuBeginGame,
    ProfileMenuCreate,
    ProfileMenuChoose,
    ProfileMenuCreateGiveName,
    SettingsMenuLocalPlayers,
    SettingsMenuRobotPlayers,
    SettingsMenuDeck,
    GameMenuPauseBackToGame,
    GameMenuPauseQuit,
    GamePlayerSkip,
    GamePlayerColorChoosed,
    GamePlayerDrawCard,
    GamePlayerReverse,
    GamePlayerMatchWon,
    GameContinue,
    GameStarted,
    GameLastCard,
    GameEvent,
    GamePlayerCards,
    GameCardsPlaced,
    CardColorRed,
    CardColorBlue,
    CardColorGreen,
    CardColorYellow,
    CardColorBlack,
    CardTypeNumber,
    CardTypeSkip,
    CardTypeReverse,
    CardTypeDraw,
    CardTypeWild
};

class Localization
{
private:
	std::unordered_map<LocalizationKey, std::string> m_dict;
    Localization() = default;
public:
	
	Localization(const Localization&) = delete;
	Localization& operator=(const Localization&) = delete;

    void init(Language lang);
	std::string get_text_from_string(LocalizationKey key);
	static Localization& get_instance();
};

