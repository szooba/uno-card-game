#include "localization.h"
#include <string>

void Localization::init(Language lang)
{
    m_dict.clear();
    switch (lang) {
    case ENGLISH:
        m_dict = {
            {LocalizationKey::SelectLanguage,            "Select a language"},
            {LocalizationKey::English,                   "English"},
            {LocalizationKey::Hungarian,                 "Hungarian"},
            {LocalizationKey::MainMenuSingleplayer,      "Single-player"},
            {LocalizationKey::MainMenuChangeLanguage,    "Change language"},
            {LocalizationKey::MainMenuChangeName,        "Change name"},
            {LocalizationKey::MainMenuExit,              "Exit"},
            {LocalizationKey::MainMenuPlayer,            "Active profile: "},
            {LocalizationKey::MainMenuProfileChange,     "Change your profile"},
            {LocalizationKey::MainMenuDelayEvents,       "Delay between events (ms)"},
            {LocalizationKey::MainMenuBeginGame,         "Play"},
            {LocalizationKey::ProfileMenuCreate,         "Create new profile"},
            {LocalizationKey::ProfileMenuChoose,         "Choose a profile"},
            {LocalizationKey::ProfileMenuCreateGiveName, "Give a name for your profile"},
            {LocalizationKey::SettingsMenuLocalPlayers,  "Number of local players"},
            {LocalizationKey::SettingsMenuRobotPlayers,  "Number of robot players"},
            {LocalizationKey::SettingsMenuDeck,          "Number of decks to start with"},
            {LocalizationKey::GameMenuPauseBackToGame,   "Continue"},
            {LocalizationKey::GameMenuPauseQuit,         "Quit"},
            {LocalizationKey::GamePlayerSkip,            "{} skipped {}"},
            {LocalizationKey::GamePlayerColorChoosed,    "{} choosed {}"},
            {LocalizationKey::GamePlayerDrawCard,        "{} draw {} card"},
            {LocalizationKey::GamePlayerReverse,         "{} reversed!"},
            {LocalizationKey::GamePlayerMatchWon,        "{} won!"},
            {LocalizationKey::GameContinue,              "Press any key to continue"},
            {LocalizationKey::GameStarted,               "Game started!"},
            {LocalizationKey::GameLastCard,              "Current card: {}"},
            {LocalizationKey::GameEvent,                 "Event: {}"},
            {LocalizationKey::GamePlayerCards,           "Your cards: "},
            {LocalizationKey::GameCardsPlaced,           "{} cards placed"},
            {LocalizationKey::CardColorRed,              "Red"},
            {LocalizationKey::CardColorBlue,             "Blue"},
            {LocalizationKey::CardColorGreen,            "Green"},
            {LocalizationKey::CardColorYellow,           "Yellow"},
            {LocalizationKey::CardColorBlack,            "Black"},
            {LocalizationKey::CardTypeNumber,            "Simple card"},
            {LocalizationKey::CardTypeSkip,              "Skip"},
            {LocalizationKey::CardTypeReverse,           "Reverse"},
            {LocalizationKey::CardTypeDraw,              "Draw"},
            {LocalizationKey::CardTypeWild,              "Wild"}

        };
        break;
    case HUNGARIAN:
        m_dict = {
            {LocalizationKey::SelectLanguage,            "Válassz nyelvet"},
            {LocalizationKey::English,                   "Angol"},
            {LocalizationKey::Hungarian,                 "Magyar"},
            {LocalizationKey::MainMenuSingleplayer,      "Egyjátékos mód"},
            {LocalizationKey::MainMenuChangeLanguage,    "Nyelvváltás"},
            {LocalizationKey::MainMenuChangeName,        "Név megváltoztatása"},
            {LocalizationKey::MainMenuExit,              "Kilépés"},
            {LocalizationKey::MainMenuPlayer,            "Aktív profil: "},
            {LocalizationKey::MainMenuProfileChange,     "Profilváltás"},
            {LocalizationKey::MainMenuDelayEvents,       "Események közötti késleltetés (ms)"},
            {LocalizationKey::MainMenuBeginGame,         "Játék indítása"},
            {LocalizationKey::ProfileMenuCreate,         "Új profil létrehozása"},
            {LocalizationKey::ProfileMenuChoose,         "Profil választása"},
            {LocalizationKey::ProfileMenuCreateGiveName, "Add meg a profilod nevét"},
            {LocalizationKey::SettingsMenuLocalPlayers,  "Helyi játékosok száma"},
            {LocalizationKey::SettingsMenuRobotPlayers,  "Robot játékosok száma"},
            {LocalizationKey::SettingsMenuDeck,          "Kezdő paklik száma"},
            {LocalizationKey::GameMenuPauseBackToGame,   "Folytatás"},
            {LocalizationKey::GameMenuPauseQuit,         "Kilépés"},
            {LocalizationKey::GamePlayerSkip,            "{} kihagyta: {}"},
            {LocalizationKey::GamePlayerColorChoosed,    "{} ezt a színt választotta: {}"},
            {LocalizationKey::GamePlayerDrawCard,        "{} húzott {} lapot"},
            {LocalizationKey::GamePlayerReverse,         "{} megfordította a kört!"},
            {LocalizationKey::GamePlayerMatchWon,        "{} nyert!"},
            {LocalizationKey::GameContinue,              "Nyomj meg egy gombot a folytatáshoz"},
            {LocalizationKey::GameStarted,               "Játék elindult!"},
            {LocalizationKey::GameLastCard,              "Jelenlegi lap: {}"},
            {LocalizationKey::GameEvent,                 "Esemény: {}"},
            {LocalizationKey::GamePlayerCards,           "Lapjaid: "},
            {LocalizationKey::GameCardsPlaced,           "{} lap lepakolva"},
            {LocalizationKey::CardColorRed,              "Piros"},
            {LocalizationKey::CardColorBlue,             "Kék"},
            {LocalizationKey::CardColorGreen,            "Zöld"},
            {LocalizationKey::CardColorYellow,           "Sárga"},
            {LocalizationKey::CardColorBlack,            "Fekete"},
            {LocalizationKey::CardTypeNumber,            "Számos lap"},
            {LocalizationKey::CardTypeSkip,              "Kimaradsz"},
            {LocalizationKey::CardTypeReverse,           "Irányváltó"},
            {LocalizationKey::CardTypeDraw,              "Húzz lapot"},
            {LocalizationKey::CardTypeWild,              "Színválasztó"}
        };
        break;
    }
}

std::string Localization::get_text_from_string(LocalizationKey key)
{
    auto iter = m_dict.find(key);
    if (iter != m_dict.end()) {
        return iter->second;
    }
    return "MISSING_TRANSLATION";
}

Localization& Localization::get_instance() {
    static Localization instance;
    return instance;
}
