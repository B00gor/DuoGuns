#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include "chatmessage.h"
#include <ftxui/component/component_base.hpp>
#include <vector>

class ChatManager {
private:
    std::vector<ChatMessage> chatMessages;

public:
    ChatManager();

    void addMessage(ChatMessage& message);
    void addSystemMessage(const Element& content);
    void addDiedGasterMessage(const Element& content);
    void addPlayerMessage(const Element& content);
    void addBotMessage(const Element& content);
    void addAttackMessage(const Element& content, bool isPlayer);

    std::vector<ChatMessage>& messages();
    void clear();

    Element renderChat();
    Component createChatComponent();
};

#endif // CHATMANAGER_H
