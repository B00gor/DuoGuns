#include "chatmanager.h"
#include "config.h"

ChatManager::ChatManager() {
    addSystemMessage("Система: Игра началась!");
    addSystemMessage("Система: Сначала выберите своего стрелка для атаки");
    addSystemMessage("Система: Затем выберите цель из правой команды");
    addSystemMessage("Система: Защитники автоматически защищают стрелков!");
    addSystemMessage("Система: Чтобы атаковать стрелка, сначала нужно победить его защитника!");
    addSystemMessage("Система: Уничтожьте всех бойцов вражеской команды, чтобы получить очко!");
}

void ChatManager::addMessage(ChatMessage& message) {
    chatMessages.push_back(message);
}

void ChatManager::addSystemMessage(const std::string& content) {
    chatMessages.emplace_back("system", text(content) | color(Config::UI::systemColor) | bold);
}

void ChatManager::addPlayerMessage(std::string& content) {
    chatMessages.emplace_back("player",
                              hbox({ text("Ты: ") | color(Config::UI::playerColor),
                                    separator(),
                                    text(content) })
                                  | border
                              );
}

void ChatManager::addBotMessage(const std::string& content) {
    chatMessages.emplace_back("bot",
                              hbox({ text("Бот: ") | color(Config::UI::botColor),
                                    separator(),
                                    text(content) })
                                  | border
                              );
}

void ChatManager::addAttackMessage(std::string& attacker, std::string& target,
                                   int damage, bool hit, bool isPlayer) {
    Element messageElement;

    if (!hit) {
        messageElement = hbox({
                             text(isPlayer ? "Ты: " : "Бот: ") | color(isPlayer ? Config::UI::playerColor : Config::UI::botColor),
                             separator(),
                             text(attacker) | color(isPlayer ? Config::UI::playerColor : Config::UI::botColor) | bold,
                             text(" промахнулся по "),
                             text(target) | color(Color::GreenYellow) | bold,
                             text("!")
                         }) | border;
    } else if (damage > 0) {
        messageElement = hbox({
                             text(isPlayer ? "Ты: " : "Бoт: ") | color(isPlayer ? Config::UI::playerColor : Config::UI::botColor),
                             separator(),
                             text(attacker) | color(isPlayer ? Config::UI::playerColor : Config::UI::botColor) | bold,
                             text(" нанес "),
                             text(std::to_string(damage)) | color(Color::Red) | bold,
                             text(" урона "),
                             text(target) | color(Color::GreenYellow) | bold,
                             text("!")
                         }) | border;
    } else {
        messageElement = hbox({
                             text(isPlayer ? "Ты: " : "Бот: ") | color(isPlayer ? Config::UI::playerColor : Config::UI::botColor),
                             separator(),
                             text(attacker) | color(isPlayer ? Config::UI::playerColor : Config::UI::botColor) | bold,
                             text(" атакует "),
                             text(target) | color(Color::Yellow) | bold,
                             text("!")
                         }) | border;
    }

    chatMessages.emplace_back(isPlayer ? "player" : "bot", messageElement);
}

std::vector<ChatMessage>& ChatManager::messages() { return chatMessages; }
void ChatManager::clear() { chatMessages.clear(); }

Element ChatManager::renderChat() {
    std::vector<Element> chatElements;
    for (auto& msg : chatMessages) {
        if (msg.type() == "player") {
            chatElements.push_back(hbox({ msg.element(), filler() }) | xflex);
        } else if (msg.type() == "bot") {
            chatElements.push_back(hbox({ filler(), msg.element() }) | xflex);
        } else {
            chatElements.push_back(hbox({ filler(), msg.element(), filler() }) | xflex);
        }
    }
    return vbox(chatElements) | yflex;
}

Component ChatManager::createChatComponent() {
    class Impl : public ComponentBase {
    private:
        ChatManager* chatManager;
        float scrollPosition = 0.0f;

    public:
        Impl(ChatManager* manager) : chatManager(manager) {
            auto content = Renderer([this] {
                return chatManager->renderChat();
            });

            auto scrollableContent = Renderer(content, [this, content] {
                return content->Render() | focusPositionRelative(0.0f, scrollPosition) | frame | flex;
            });

            SliderOption<float> optionY;
            optionY.value = &scrollPosition;
            optionY.min = 0.f;
            optionY.max = 1.f;
            optionY.increment = 0.05f;
            optionY.direction = Direction::Down;
            optionY.color_active = Color::Yellow;
            optionY.color_inactive = Color::YellowLight;
            auto scrollbar = Slider(optionY);

            Add(Container::Horizontal({
                scrollableContent | flex,
                scrollbar
            }));
        }
    };

    return Make<Impl>(this);
}
