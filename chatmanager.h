#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include <vector>
#include <string>
#include <memory>
#include "chatmessage.h"
#include <ftxui/component/component.hpp>

using namespace ftxui;

class ChatManager {
private:
    std::vector<ChatMessage> chatMessages;

public:
    ChatManager();
    void addMessage(ChatMessage& message);
    void addSystemMessage(const std::string& content);
    void addPlayerMessage(std::string& content);
    void addBotMessage(const std::string& content);
    void addAttackMessage(std::string& attacker, std::string& target, int damage = 0, bool hit = true, bool isPlayer = true); std::vector<ChatMessage>& messages() ;
    void clear();
    Element renderChat();
    Component createChatComponent();
};

#endif // CHAT_MANAGER_H
