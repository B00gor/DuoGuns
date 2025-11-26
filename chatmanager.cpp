#include "chatmanager.h"
#include "config.h"

ChatManager::ChatManager() {
    addSystemMessage(text("Система: Игра началась!") | color(Config::UI::systemColor) | bold);
    addSystemMessage(text("Система: Выберите стрелка для атаки") | color(Config::UI::systemColor) | bold);
    addSystemMessage(text("Система: Выберите цель из правой команды") | color(Config::UI::systemColor) | bold);
    addSystemMessage(text("Система: Защитники защищают стрелков!") | color(Config::UI::systemColor) | bold);
}

void ChatManager::addMessage(ChatMessage& message) {
    chatMessages.push_back(message);
}

void ChatManager::addSystemMessage(const Element& content) {
    chatMessages.emplace_back("system", content);
}

void ChatManager::addDiedGasterMessage(const Element& content) {
    auto messageElement = window(
        text("") | hcenter | bold,
        content
        );
    chatMessages.emplace_back("center", messageElement);
}

void ChatManager::addPlayerMessage(const Element& content) {
    auto messageElement = window(
        text("") | hcenter | bold,
        content
        );
    chatMessages.emplace_back("player", messageElement);
}

void ChatManager::addBotMessage(const Element& content) {
    auto messageElement = window(
        text("") | hcenter | bold,
        content
        );
    chatMessages.emplace_back("bot", messageElement);
}

void ChatManager::addAttackMessage(const Element& content, bool isPlayer) {
    auto messageElement = window(
        text(isPlayer ? "АТАКА ИГРОКА" : "АТАКА БОТА") | hcenter | bold |
            color(isPlayer ? Config::UI::playerColor : Config::UI::botColor),
        content
        );
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
            scrollPosition = 1.0f;
        }
    };

    return Make<Impl>(this);
}
