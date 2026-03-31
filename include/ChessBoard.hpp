#pragma once
#include "Constants.hpp"

#include <SFML/Audio.hpp>
#include <array>
#include <optional>
#include <unordered_map>
#include <vector>

class ChessBoard
{
public:
    ChessBoard();
    void update(float dt);
    void draw(sf::RenderWindow &target);
    void handleHover(const sf::Vector2i &mousePos);
    void handleClick(const sf::Vector2i &mousePos);

private:
    // Logic State
    bool isWhiteTurn = true;
    int hoveredIndex = -1, selectedIndex = -1;
    bool inCheck = false, isCheckmate = false;
    int enPassantIndex = -1;

    // --- TIMERS (Set your time here) ---
    float whiteTime = 600.0f;
    float blackTime = 600.0f;

    int whiteScore = 0, blackScore = 0;
    std::vector<int8_t> capturedByWhite; // Contains Black pieces
    std::vector<int8_t> capturedByBlack; // Contains White pieces

    std::array<int8_t, 64> boardState = {
        -5, -4, -3, -9, -7, -3, -4, -5, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0,
        0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  5,  4,  3,  9,  7, 3, 4, 5};

    // Castling Memory
    bool wKMove = false, wRLMove = false, wRRMove = false;
    bool bKMove = false, bRLMove = false, bRRMove = false;

    // Components
    std::vector<sf::RectangleShape> squares;
    std::vector<sf::Sprite> pieceSprites;
    std::unordered_map<int8_t, sf::Texture> textureMap;
    sf::Texture backgroundTexture;
    sf::Font font;
    sf::SoundBuffer moveBuffer;
    sf::Sound moveSound;
    std::vector<sf::Text> labels;
    sf::CircleShape moveHintCircle;
    std::vector<MoveHint> possibleMoves;

    // Methods
    void loadAssets();
    void initLabels();
    void syncPiecesWithBoard();
    void calculatePossibleMoves(int startIdx);
    std::optional<int> getIndexFromMouse(const sf::Vector2i &mPos);
    int getPieceValue(int8_t piece);

    // Engine (MoveLogic.cpp)
    bool validateMove(int s, int e, bool ignoreCheck = false);
    bool simulateMoveAndCheck(int s, int e, bool white);
    bool isKingInCheck(bool white);
    bool hasLegalMoves(bool white);
    bool isPathClear(int s, int e);
    bool valPawn(int s, int e, int8_t p);
    bool valRook(int s, int e);
    bool valBishop(int s, int e);
    bool valKnight(int s, int e);
    bool valQueen(int s, int e);
    bool valKing(int s, int e);

    bool isWhite(int8_t p) { return p > 0; }
    bool isSameTeam(int8_t p1, int8_t p2)
    {
        return (p1 != 0 && p2 != 0) && ((p1 > 0 && p2 > 0) || (p1 < 0 && p2 < 0));
    }
};