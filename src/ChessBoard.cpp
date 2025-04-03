#include "ChessBoard.hpp"

ChessBoard::ChessBoard(){
    this->load();

    this->squares.reserve(64);
    this->pieces.reserve(33);

    for(size_t row = 0; row < 8; row++){
        for(size_t col = 0; col < 8; col++){
            this->squares.emplace_back(sf::RectangleShape({PIXEL_WIDTH, PIXEL_WIDTH}));
        
            sf::RectangleShape& rec = this->squares.back();

            rec.setPosition({PIXEL_WIDTH * col + BOARD_OFFSET_X, PIXEL_WIDTH * row + BOARD_OFFSET_Y});
            rec.setFillColor(((row + col) % 2 == 0) ? sf::Color(222, 184, 135) : sf::Color(139, 69, 19));
        }
    }

    for (size_t row = 0; row < 8; row++) {
        for (size_t col = 0; col < 8; col++) {
            int index = row * 8 + col;  // Map 2D coordinates to 1D array index
            int8_t pieceCode = Board[index];
            
            sf::Sprite piece(this->textureMap[names[0]]);
            piece.setPosition({PIXEL_WIDTH * col + BOARD_OFFSET_X, PIXEL_WIDTH * row + BOARD_OFFSET_Y});
            piece.setScale({2, 2});
            
            bool isSet = false;
            if (pieceCode != 0) {  // Only create a sprite if there’s a piece
                isSet = true;
                if(pieceCode == -3)
                    piece.setTexture(this->textureMap[names[0]]);
                else if(pieceCode == -7)
                    piece.setTexture(this->textureMap[names[1]]);
                else if(pieceCode == -4)
                    piece.setTexture(this->textureMap[names[2]]);
                else if(pieceCode == -1)
                    piece.setTexture(this->textureMap[names[3]]);
                else if(pieceCode == -9)
                    piece.setTexture(this->textureMap[names[4]]);
                else if(pieceCode == -5)
                    piece.setTexture(this->textureMap[names[5]]);

                else if(pieceCode == 3)
                    piece.setTexture(this->textureMap[names[6]]);
                else if(pieceCode == 7)
                    piece.setTexture(this->textureMap[names[7]]);
                else if(pieceCode == 4)
                    piece.setTexture(this->textureMap[names[8]]);
                else if(pieceCode == 1)
                    piece.setTexture(this->textureMap[names[9]]);
                else if(pieceCode == 9)
                    piece.setTexture(this->textureMap[names[1]]);
                else if(pieceCode == 5)
                    piece.setTexture(this->textureMap[names[1]]);
                
                if(isSet)
                    this->pieces.emplace_back(piece);
            }
        }
    }

}

void ChessBoard::load(){
    
    for(const auto& name : names){
        sf::Texture texture;
        if (texture.loadFromFile("./assets/images/" + name + ".png")) {
            texture.setSmooth(true);
            textureMap[name] = std::move(texture);
        } 
        else {
            std::cerr << "Failed to load " << name << std::endl;
        }
    }
}

void ChessBoard::draw(sf::RenderWindow& target){//){
    
    sf::Sprite sprite(textureMap[names[12]]);
    target.draw(sprite);
   
    for(const auto& rec : this->squares)
        target.draw(rec);
    
    for(const auto& piece : this->pieces)
        target.draw(piece);
} 