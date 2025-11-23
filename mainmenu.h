#ifndef MAINMENU_H
#define MAINMENU_H

#include <functional>
#include <ftxui/component/component.hpp>
#include "config.h"

using namespace ftxui;

class MainMenu {
public:
    MainMenu() = default;
    Component create(std::function<void(int)> onMenuSelect);
};

#endif // MAINMENU_H
