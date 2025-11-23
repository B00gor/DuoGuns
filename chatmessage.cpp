#include "chatmessage.h"

ChatMessage::ChatMessage(const std::string& type, const Element& element)
    : messageType(type), messageElement(element) {}

std::string& ChatMessage::type() { return messageType; }
Element& ChatMessage::element() { return messageElement; }
