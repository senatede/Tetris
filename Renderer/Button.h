#pragma once
#include <SFML/Graphics.hpp>
#include <map>

struct Button {
    sf::RectangleShape shape;
    sf::Text text;

    Button(const std::string& str, sf::Font& font, sf::Vector2f pos, sf::Vector2f size);
    void draw(sf::RenderTarget& target) const;
    bool isClicked(sf::Vector2f mousePos) const;
};