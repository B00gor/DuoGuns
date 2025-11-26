#include "mainmenu.h"

Component MainMenu::create(std::function<void(int)> onMenuSelect) {
    auto menuContainer = Container::Vertical({});

    auto singleplayerBtn = Button("Одиночная игра", [=] { onMenuSelect(1); }, Config::UI::getMenuButtonStyle());
    auto multiplayerBtn = Button("Мультиплеер", [=] { onMenuSelect(2); }, Config::UI::getMenuButtonStyleError());
    auto settingsBtn = Button("Настройки", [=] { onMenuSelect(3); }, Config::UI::getMenuButtonStyle());
    auto exitBtn = Button("Выход", [=] { onMenuSelect(0); }, Config::UI::getMenuButtonStyle());

    menuContainer->Add(singleplayerBtn);
    menuContainer->Add(multiplayerBtn);
    menuContainer->Add(settingsBtn);
    menuContainer->Add(exitBtn);

    return Renderer(menuContainer, [=] {
        auto asciiArt = vbox({
            text("██████╗ ██╗   ██╗ ██████╗  ██████╗ ██╗   ██╗███╗   ██╗███████╗") | color(Config::UI::cyanColor) | center,
            text("██╔══██╗██║   ██║██╔═══██╗██╔════╝ ██║   ██║████╗  ██║██╔════╝") | color(Config::UI::cyanColor) | center,
            text("██║  ██║██║   ██║██║   ██║██║  ███╗██║   ██║██╔██╗ ██║███████╗") | color(Config::UI::cyanColor) | center,
            text("██║  ██║██║   ██║██║   ██║██║   ██║██║   ██║██║╚██╗██║╚════██║") | color(Config::UI::cyanColor) | center,
            text("██████╔╝╚██████╔╝╚██████╔╝╚██████╔╝╚██████╔╝██║ ╚████║███████║") | color(Config::UI::cyanColor) | center,
            text("╚═════╝  ╚═════╝  ╚═════╝  ╚═════╝  ╚═════╝ ╚═╝  ╚═══╝╚══════╝") | color(Config::UI::cyanColor) | center,
            text(""),
            text("████─████─███─████──████─████────████─█──█─████─███─███─█───█─████") | color(Color::Red) | center,
            text("█────█──█─█───█──██─█──█─█──█────█──█─█──█─█──█──█──█───██─██─█──█") | color(Color::Red) | center,
            text("████─█──█─███─████──████─████────█────█─██─█─────█──███─█─█─█─████") | color(Color::Red) | center,
            text("█──█─█──█─█───█──██─█──█──█─█────█──█─██─█─█──█──█──█───█───█─█──█") | color(Color::Red) | center,
            text("████─████─███─████──█──█─█──█────████─█──█─████──█──███─█───█─█──█") | color(Color::Red) | center,
            text(""),
            text("")
        });

        auto menuBox = vbox({
                           text("ГЛАВНОЕ МЕНЮ") | bold | center,
                           separator(),
                           singleplayerBtn->Render(),
                           multiplayerBtn->Render(),
                           settingsBtn->Render(),
                           exitBtn->Render(),
                           separator(),
                           text("Управление: ↑↓ Enter | Выход: ESC, Q или Ctrl+C") | dim | center
                       }) | border | center;

        return vbox({ asciiArt, menuBox }) | center;
    });
}
