#include "mainmenu.h"

Component MainMenu::create(std::function<void(int)> onMenuSelect) {
    auto menuContainer = Container::Vertical({});

    auto menuButtonStyle = ButtonOption::Ascii();
    menuButtonStyle.transform = [](const EntryState& state) {
        auto element = text(state.label) | center | flex;
        if (state.focused) {
            element = element | borderDouble | color(Color::White);
        } else {
            element = element | border;
        }
        return element;
    };

    auto singleplayerBtn = Button("Одиночная игра", [=] { onMenuSelect(1); }, menuButtonStyle);
    auto multiplayerBtn = Button("Мультиплеер", [=] { onMenuSelect(2); }, menuButtonStyle);
    auto settingsBtn = Button("Настройки", [=] { onMenuSelect(3); }, menuButtonStyle);
    auto exitBtn = Button("Выход", [=] { onMenuSelect(0); }, menuButtonStyle);

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
