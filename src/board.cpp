#include "Board.hpp"

Board::Board(){
    this->ChessBoard.reserve(64);
    for(size_t row = 0; row < 8; row++){
        for(size_t col = 0; col < 8; col++){
            this->ChessBoard.emplace_back(sf::RectangleShape({PIXEL_WIDTH, PIXEL_WIDTH}));
        
            sf::RectangleShape& rec = this->ChessBoard.back();

            rec.setPosition({PIXEL_WIDTH * row + BOARD_OFFSET_X, PIXEL_WIDTH * col + BOARD_OFFSET_Y});

            rec.setFillColor(((row + col) % 2 == 0) ? sf::Color(222, 184, 135) : sf::Color(139, 69, 19));
        }
    }
}

std::optional<sf::Sprite> Board::load(const std::filesystem::path& path){
        
    if (!this->texture.loadFromFile(path)) {
        std::cerr << "Failed to load texture!" << std::endl;
        return std::nullopt; // Handle error
    }

    return sf::Sprite(this->texture);
    
}

void Board::draw(sf::RenderWindow& target, sf::Sprite& sprite){
    target.draw(sprite);
   
    for(const auto& rec : this->ChessBoard)
        target.draw(rec);
} 