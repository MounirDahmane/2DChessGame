#pragma once

#include "ChessBoard.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>

class ChessBoard;

/**
 * @brief Handles all SFML rendering, asset loading, and visual effects.
 * Separates graphics concerns from game logic.
 */
class ChessboardRenderer {
public:
    explicit ChessboardRenderer(ChessBoard& board);
    
    // =========================================================================
    // Rendering
    // =========================================================================
    void draw(sf::RenderWindow& target);
    
    void loadAssets();
    void initLabels();
    void setupMenuUI();
    void setupInGameUI();
    void setupBoardSquares();
    void setupPromotionDialog(int8_t piece);
    void applyTheme();
    
    // =========================================================================
    // Asset Access
    // =========================================================================
    const sf::Texture& getTextureForPiece(int8_t piece) const { return textureMap.at(piece); }
    
    // =========================================================================
    // Sound Effects
    // =========================================================================
    void playMoveSound();
    void playCaptureSound();
    void playCheckSound();

private:
    ChessBoard& board;
    
    // =========================================================================
    // SFML UI Elements
    // =========================================================================
    // Menu Buttons & Text
    sf::RectangleShape pvpButton, aiButton, diffButton, themeButton, incButton, timeButton, exitButton;
    sf::Text pvpText, aiText, diffText, menuTitle, themeText, incText, timeText, exitText;
    
    // In-Game UI
    sf::RectangleShape undoButton, menuButton;
    sf::Text undoText, menuBtnText, thinkingText;
    
    // Promotion Modal
    sf::RectangleShape promoBox;
    std::vector<sf::Sprite> promoSprites;
    std::vector<int8_t> promoOptions;

    // Board Graphics
    std::vector<sf::RectangleShape> squares;
    std::vector<sf::Sprite> pieceSprites;
    std::vector<sf::Text> labels;
    sf::CircleShape moveHintCircle;

    // =========================================================================
    // SFML Assets
    // =========================================================================
    std::unordered_map<int8_t, sf::Texture> textureMap;
    sf::Texture backgroundTexture;
    sf::Font font;

    sf::SoundBuffer moveSoundBuffer, captureSoundBuffer, checkSoundBuffer;
    sf::Sound moveSound, captureSound, checkSound;
    
    // =========================================================================
    // Drawing Methods
    // =========================================================================
    void drawMenu(sf::RenderWindow& target);
    void drawBoard(sf::RenderWindow& target);
    void drawSidebar(sf::RenderWindow& target);
    void drawPieces(sf::RenderWindow& target);
    void drawPromotionDialog(sf::RenderWindow& target);
    void applyScreenShake(sf::RenderWindow& target);
    void syncPiecesWithBoard();
    
    // =========================================================================
    // UI Helpers
    // =========================================================================
    void centerText(sf::Text& text, float x, float y);

    friend class ChessBoard;
    friend class ChessboardUI;
};
