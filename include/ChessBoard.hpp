#pragma once
#include "Constants.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class ChessBoard
{
public:
    enum class GameState
    {
        MENU,
        PLAYING
    };

    struct Move
    {
        int from;
        int to;
        int capturedPiece;
        bool isEnPassant;
    };

    struct MoveHint
    {
        int index;
        bool isCapture;
    };

    ChessBoard();
    void update(float dt);
    void draw(sf::RenderWindow &target);
    void handleHover(const sf::Vector2i &mousePos);
    void handleClick(const sf::Vector2i &mousePos);

    void aiMove();
    void resetGame();

    void setAiMode(bool mode) { isAiGame = mode; }
    bool getIsAiGame() const { return isAiGame; }
    bool getIsWhiteTurn() const { return isWhiteTurn; }
    bool getIsCheckmate() const { return isCheckmate; }
    bool getIsPromoting() const { return isPromoting; }
    GameState getState() const { return currentState; }

    Move makeMove(int s, int e);
    void undoMove(Move m);

private:
    GameState currentState = GameState::MENU;
    bool isAiGame = false;
    bool isWhiteTurn = true;
    int hoveredIndex = -1, selectedIndex = -1;
    bool inCheck = false, isCheckmate = false;
    int enPassantIndex = -1;

    bool isPromoting = false;
    int promotionIdx = -1;

    float shakeTimer = 0.0f;
    float shakeIntensity = 0.0f;

    float whiteTime = 600.0f;
    float blackTime = 600.0f;
    int whiteScore = 0, blackScore = 0;

    std::vector<int8_t> capturedByWhite;
    std::vector<int8_t> capturedByBlack;
    std::vector<std::string> moveHistory;

    std::array<int8_t, 64> boardState;

    bool wKMove = false, wRLMove = false, wRRMove = false;
    bool bKMove = false, bRLMove = false, bRRMove = false;

    // UI elements
    sf::RectangleShape pvpButton;
    sf::RectangleShape aiButton;
    sf::Text pvpText;
    sf::Text aiText;
    sf::Text menuTitle;

    std::vector<sf::RectangleShape> squares;
    std::vector<sf::Sprite> pieceSprites;
    std::unordered_map<int8_t, sf::Texture> textureMap;
    sf::Texture backgroundTexture;
    sf::Font font;

    sf::SoundBuffer moveBuf, captureBuf;
    sf::Sound moveSound, captureSound, checkSound;

    std::vector<sf::Text> labels;
    sf::CircleShape moveHintCircle;
    std::vector<MoveHint> possibleMoves;

    void loadAssets();
    void initLabels();
    void syncPiecesWithBoard();
    void calculatePossibleMoves(int startIdx);
    std::optional<int> getIndexFromMouse(const sf::Vector2i &mPos);
    int getPieceValue(int8_t piece);
    void addMoveToLog(int start, int end, int8_t piece);
    void triggerShake(float intensity, float duration);

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

    int minimax(int depth, bool isMaximizing, int alpha, int beta);
    int evaluateBoard();
};