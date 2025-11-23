#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <string>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

class ChatMessage {
private:
    std::string messageType;
    Element messageElement;

public:
    ChatMessage(const std::string& type, const Element& element);
    std::string& type();
    Element& element();
};

#endif // CHATMESSAGE_H
