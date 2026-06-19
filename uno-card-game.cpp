#include "main_menu.h"
#include "localization.h"
#include <windows.h>

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    
    Localization::get_instance().init(ENGLISH);
    MainMenu mm = MainMenu();
    mm.language_selection();
    mm.create_profile_menu();
    mm.main();
}


