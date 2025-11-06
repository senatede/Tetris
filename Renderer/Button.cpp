#include "Button.h"

Button::Button(const std::string& str, sf::Font& font, sf::Vector2f pos, sf::Vector2f size) {
    shape.setPosition(pos);
    shape.setSize(size);
    shape.setFillColor(sf::Color(0, 0, 0, 0));
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(2.f);

    text.setString(str);
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);

    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(pos.x + size.x / 2.0f, pos.y + size.y / 2.0f);
}

void Button::draw(sf::RenderTarget& target) const {
    target.draw(shape);
    target.draw(text);
}

bool Button::isClicked(const sf::Vector2f mousePos) const {
    return shape.getGlobalBounds().contains(mousePos);
}