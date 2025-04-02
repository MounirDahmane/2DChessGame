#pragma once

#include <SFML/Graphics.hpp> 
#include <vector>
#include <iostream>

#define PIXEL_WIDTH 100.0f
#define BOARD_OFFSET_Y 50.0f
#define BOARD_OFFSET_X 50.0f

class Board
{
public:
    Board();

    std::optional<sf::Sprite> load(const std::filesystem::path& path);

    void draw(sf::RenderWindow& target, sf::Sprite& sprite);

private:
    sf::Texture texture;
    std::vector<sf::RectangleShape> ChessBoard;
};


    


    