#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <array>
#include <unordered_map>

#define PIXEL_WIDTH    100.0f
#define BOARD_OFFSET_Y 50.0f
#define BOARD_OFFSET_X 50.0f

#define BLACK_SQUARE   sf::Color(222, 184, 135)
#define WHITE_SQUARE   sf::Color(139, 69, 19)
#define HIGH_LIGHTED_SQUARE   sf::Color(200, 69, 19)

#define BLACK_BISHOP        -3    
#define BLACK_KING          -7
#define BLACK_KNIGHT        -3
#define BLACK_PAWN          -1
#define BLACK_QUEEN         -9
#define BLACK_ROOK          -5

#define White_BISHOP        3    
#define White_KING          7
#define White_KNIGHT        3
#define White_PAWN          1
#define White_QUEEN         9
#define White_ROOK          5

inline std::array<std::string, 13> names{
    "blackBishop", "blackKing", "blackKnight", "blackPawn", "blackQueen", "blackRook",
    "whiteBishop", "whiteKing", "whiteKnight", "whitePawn", "whiteQueen", "whiteRook", 
    "backGround"
};

inline std::array<int8_t, 64> Board{
    -5, -4, -3, -9, -7, -3, -4, -5,
    -1, -1, -1, -1, -1, -1, -1, -1,
     0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,
     1,  1,  1,  1,  1,  1,  1,  1,
     5,  4,  3,  9,  7,  3,  4,  5
};

class ChessBoard
{
public:
    ChessBoard();

    void load();

    void draw(sf::RenderWindow& target);

    void setColor(const sf::Vector2i& vec);
private:

    struct 
    {
        int8_t Index;
        sf::Color Color;
    } highLightedSquareInfo{-1, WHITE_SQUARE};
    
    std::vector<sf::RectangleShape> squares;
    std::vector<sf::Sprite> pieces;
    std::unordered_map<std::string, sf::Texture> textureMap;
};


    


    