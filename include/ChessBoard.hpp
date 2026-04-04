/**
 * @file ChessBoard.hpp
 * @brief Main controller class for the E2-E4 Chess Engine.
 * * This file contains the primary ChessBoard class which manages the game state,
 * coordinates user input, renders the board, and communicates with the AI module.
 */

#pragma once

#include "Constants.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <atomic>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class ChessboardAI; // Forward declaration

/**
 * @brief Represents a chess move and necessary data to undo it.
 */
struct Move
{
    int from;              ///< Starting square index (0-63)
    int to;                ///< Destination square index (0-63)
    int8_t capturedPiece;  ///< The piece that was captured (if any)
    int8_t movingPiece;    ///< The piece that is moving (prevents ghost pieces during AI undo)
    bool isEnPassant;      ///< Flag indicating if the move was an En Passant capture
    bool isCastling;       ///< Flag indicating if the move was a castling move
    int8_t promotionPiece; ///< The piece code chosen for pawn promotion
};

/**
 * @brief Represents a visual hint for a possible move.
 */
struct MoveHint
{
    int index;      ///< Square index for the hint
    bool isCapture; ///< True if the move results in a capture (changes hint color)
};

/**
 * @brief Defines the search depth/difficulty for the AI opponent.
 */
enum class AIDifficulty
{
    EASY = 1,
    MEDIUM = 3,
    HARD = 4
};

/**
 * @brief A complete snapshot of the game state used for the Undo functionality.
 */
struct StateSnapshot
{
    std::array<int8_t, 64> boardState;
    bool isWhiteTurn;
    bool inCheck;
    int enPassantIndex;
    int whiteScore;
    int blackScore;
    int halfMoveClock;
    bool whiteKingMoved;
    bool whiteRookRightMoved;
    bool whiteRookLeftMoved;
    bool blackKingMoved;
    bool blackRookRightMoved;
    bool blackRookLeftMoved;
    std::vector<int8_t> capturedByWhite;
    std::vector<int8_t> capturedByBlack;
    std::vector<std::string> moveHistory;
};

/**
 * @class ChessBoard
 * @brief The core game controller handling rules, rendering, UI, and state.
 */
class ChessBoard
{
    friend class ChessboardAI; // Grants the new AI class access to private variables
public:
    enum class GameState
    {
        MENU,
        PLAYING
    };

    // =========================================================================
    // Core Lifecycle & Engine
    // =========================================================================
    ChessBoard();
    ~ChessBoard();

    void update(float dt);
    void draw(sf::RenderWindow &target);
    void resetGame();

    // =========================================================================
    // Input & Interaction
    // =========================================================================
    void handleHover(const sf::Vector2i &mousePos);
    void handleClick(const sf::Vector2i &mousePos);
    void handleRelease(const sf::Vector2i &mousePos);
    void promoteSelectedPawn(int8_t promotionPiece);

    // =========================================================================
    // State Accessors (Getters & Setters)
    // =========================================================================
    bool getIsAiGame() const { return isAiGame; }
    bool getIsWhiteTurn() const { return isWhiteTurn; }
    bool getIsCheckmate() const { return isCheckmate; }
    bool getIsPromoting() const { return isPromoting; }
    bool getIsInCheck() const { return inCheck; }
    bool getShouldExit() const { return shouldExit; }
    GameState getState() const { return currentState; }

    void setAiMode(bool mode) { isAiGame = mode; }
    void setGameState(GameState state) { currentState = state; }

    // =========================================================================
    // External Move Control (Used by AI & Main Loop)
    // =========================================================================
    Move makeMove(int s, int e);
    void undoMove(const Move &m);

private:
    std::unique_ptr<ChessboardAI> aiOpponent; ///< Pointer to our externalized AI module

    // =========================================================================
    // Core Game State
    // =========================================================================
    GameState currentState = GameState::MENU;
    bool isAiGame = false;
    bool shouldExit = false;
    bool isWhiteTurn = true;
    bool gameStarted = false;

    int hoveredIndex = -1;
    int selectedIndex = -1;
    int enPassantIndex = -1;
    int halfMoveClock = 0;

    bool inCheck = false;
    bool isCheckmate = false;
    bool isStalemate = false;

    bool isPromoting = false;
    int promotionIndex = -1;

    // =========================================================================
    // Visual Effects & Timers
    // =========================================================================
    float shakeTimer = 0.0f;
    float shakeIntensity = 0.0f;

    std::atomic<bool> isAiThinking{false};
    std::future<Move> aiFuture;
    std::array<int8_t, 64>
        renderBoardState; ///< Safe copy of board for rendering during async AI calculations

    int selectedTime = 10;
    float whiteTime = 600.0f;
    float blackTime = 600.0f;
    int whiteScore = 0;
    int blackScore = 0;

    // =========================================================================
    // Histories & Castling Rights
    // =========================================================================
    std::vector<int8_t> capturedByWhite;
    std::vector<int8_t> capturedByBlack;
    std::vector<std::string> moveHistory;
    std::array<int8_t, 64> boardState;

    bool whiteKingMoved = false, whiteRookRightMoved = false, whiteRookLeftMoved = false;
    bool blackKingMoved = false, blackRookRightMoved = false, blackRookLeftMoved = false;

    AIDifficulty aiDifficulty = AIDifficulty::MEDIUM;
    int currentTheme = 0;
    bool useIncrement = false;

    std::vector<StateSnapshot> stateHistory;

    // Animation & Drag state
    bool isAnimating = false;
    float animProgress = 0.0f;
    int animFrom = -1;
    int animTo = -1;
    int8_t animPieceCode = 0;
    sf::Sprite animSprite;

    bool isDragging = false;
    int dragIndex = -1;
    sf::Vector2f currentMousePos;

    // =========================================================================
    // SFML UI Elements & Assets
    // =========================================================================
    sf::RectangleShape pvpButton, aiButton, diffButton, themeButton, incButton, timeButton,
        exitButton;
    sf::RectangleShape undoButton, menuButton;
    sf::Text pvpText, aiText, diffText, menuTitle, undoText, themeText, incText, timeText, exitText,
        menuBtnText;
    sf::Text thinkingText;

    sf::RectangleShape promoBox;
    std::vector<sf::Sprite> promoSprites;
    std::vector<int8_t> promoOptions;

    std::vector<sf::RectangleShape> squares;
    std::vector<sf::Sprite> pieceSprites;
    std::unordered_map<int8_t, sf::Texture> textureMap;
    sf::Texture backgroundTexture;
    sf::Font font;

    sf::SoundBuffer moveSoundBuffer, captureSoundBuffer, checkSoundBuffer;
    sf::Sound moveSound, captureSound, checkSound;

    std::vector<sf::Text> labels;
    sf::CircleShape moveHintCircle;
    std::vector<MoveHint> possibleMoves;

    // =========================================================================
    // Internal Setup & Initialization
    // =========================================================================
    void setupMenuUI();
    void setupInGameUI();
    void loadAssets();
    void initLabels();
    void syncPiecesWithBoard();

    // =========================================================================
    // Rendering & Drawing
    // =========================================================================
    void drawMenu(sf::RenderWindow &target);
    void drawBoard(sf::RenderWindow &target);
    void drawSidebar(sf::RenderWindow &target);
    void drawPieces(sf::RenderWindow &target);
    void drawPromotionDialog(sf::RenderWindow &target);
    void applyScreenShake(sf::RenderWindow &target);
    void centerText(sf::Text &text, float x, float y);

    // =========================================================================
    // Game Logic & State Management
    // =========================================================================
    void executeMoveWithEffects(int from, int to);
    void handleCapture(int target);
    void handleEnPassantCapture(int from, int to);
    void handleCastling(int from, int to);
    void updateCastlingRights(int from);
    void updateEnPassantSquare(int from, int to, int8_t piece);
    void saveSnapshot();
    void performUndo();
    void applyTheme();
    void triggerShake(float intensity, float duration);
    void addMoveToLog(int from, int to, int8_t piece);

    // =========================================================================
    // Move Validation & Pathfinding
    // =========================================================================
    void calculatePossibleMoves(int startIdx);
    std::optional<int> getIndexFromMouse(const sf::Vector2i &mPos);

    bool validateMove(int s, int e, bool ignoreCheck = false);
    bool simulateMoveAndCheck(int s, int e, bool white);
    bool isKingInCheck(bool white);
    bool hasLegalMoves(bool white);
    bool isPathClear(int s, int e);

    bool validatePawn(int from, int to, int8_t piece);
    bool validateRook(int from, int to);
    bool validateBishop(int from, int to);
    bool validateKnight(int from, int to);
    bool validateQueen(int from, int to);
    bool validateKing(int from, int to);

    // =========================================================================
    // Utility Helpers
    // =========================================================================
    bool isWhite(int8_t p) const { return p > 0; }
    bool isSameTeam(int8_t p1, int8_t p2) const
    {
        return (p1 != 0 && p2 != 0) && ((p1 > 0 && p2 > 0) || (p1 < 0 && p2 < 0));
    }
    int getPieceValue(int8_t piece) const;
};