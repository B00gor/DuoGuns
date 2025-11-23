#ifndef APPLICATION_H
#define APPLICATION_H

#include "battlecontroller.h"
#include "battleview.h"
#include "teamselectionscreen.h"
#include "mainmenu.h"
#include "config.h"
#include <memory>

class Application {
private:
    int currentScreen = 0;
    bool shouldExit = false;

public:
    void run();

private:
    void showMainMenu();
    void showSingleplayer();
    void showMultiplayer();
    void showSettings();
};

#endif // APPLICATION_H
