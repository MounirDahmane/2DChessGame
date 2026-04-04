#pragma once

#include "Constants.hpp"
#include <SFML/Graphics.hpp>
#include <array>
#include <atomic>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// Forward declarations
class ChessboardRenderer;
class ChessboardAI;
class ChessboardUI;

/**
 * @brief Represents a chess move and necessary data to undo it.
 */
struct Move {
    int from;               ///< Starting square index (0-63)
    int to;                 ///< Destination square index (0-63)
    int8_t capturedPiece;   ///< The piece that was captured (if any)
    int8_t movingPiece;     ///< The piece that is moving (prevents ghost pieces during AI undo)
    bool isEnPassant;       ///< Flag indicating if the move was an En Passant capture
    bool isCastling;        ///< Flag indicating if the move was a castling move
    int8_t promotionPiece;  ///< The piece code chosen for pawn promotion
};

/**
 * @brief Represents a visual hint for a possible move.
 */
struct MoveHint {
    int index;              ///< Square index for the hint
    bool isCapture;         ///< True if the move results in a capture (changes hint color)
};

/**
 * @brief Defines the search depth/difficulty for the AI opponent.
 */
enum class AIDifficulty { 
    EASY = 1, 
    MEDIUM = 3, 
    HARD = 4 
};

/**
 * @brief A complete snapshot of the game state used for the Undo functionality.
 */
struct StateSnapshot {
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
 * @brief Main chess engine and game coordinator.
 * Manages game state and delegates rendering to ChessboardRenderer,
 * AI logic to ChessboardAI, and input handling to ChessboardUI.
 */
class ChessBoard {
public:
    enum class GameState { MENU, PLAYING };

    // =========================================================================
    // Core Lifecycle & Engine
    // =========================================================================
    ChessBoard();
    ~ChessBoard();
    
    void update(float dt);
    void draw(sf::RenderWindow& target);
    void resetGame();

    // =========================================================================
    // Input & Interaction (Delegates to ChessboardUI)
    // =========================================================================
    void handleHover(const sf::Vector2i& mousePos);
    void handleClick(const sf::Vector2i& mousePos);
    void handleRelease(const sf::Vector2i& mousePos); 
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
    void undoMove(const Move& m);

    // =========================================================================
    // Board State Access (for AI and Renderer)
    // =========================================================================
    const std::array<int8_t, 64>& getBoardState() const { return boardState; }
    const std::array<int8_t, 64>& getRenderBoardState() const { return renderBoardState; }

private:
    friend class ChessboardRenderer;
    friend class ChessboardAI;
    friend class ChessboardUI;

    // =========================================================================
    // Core Game State
    // =========================================================================
    GameState currentState = GameState::MENU;
    std::array<int8_t, 64> boardState;
    std::array<int8_t, 64> renderBoardState; ///< Safe copy of board for rendering during async AI calculations
    
    bool isAiGame = false;
    bool shouldExit = false;
    bool gameStarted = false; 
    bool isWhiteTurn = true;
    
    bool inCheck = false;
    bool isCheckmate = false;
    bool isStalemate = false; 
    
    int enPassantIndex = -1;
    int halfMoveClock = 0; 

    // Promotion State
    bool isPromoting = false;
    int promotionIndex = -1;

    // Castling Rights
    bool whiteKingMoved = false, whiteRookRightMoved = false, whiteRookLeftMoved = false;
    bool blackKingMoved = false, blackRookRightMoved = false, blackRookLeftMoved = false;

    // =========================================================================
    // Timers & Scoring
    // =========================================================================
    int selectedTime = 10;
    float whiteTime = 600.0f;
    float blackTime = 600.0f;
    int whiteScore = 0;
    int blackScore = 0;

    // =========================================================================
    // Histories & Snapshots
    // =========================================================================
    std::vector<int8_t> capturedByWhite;
    std::vector<int8_t> capturedByBlack;
    std::vector<std::string> moveHistory;
    std::vector<StateSnapshot> stateHistory;

    // =========================================================================
    // AI & Multithreading
    // =========================================================================
    std::atomic<bool> isAiThinking{false};
    std::future<Move> aiFuture;
    AIDifficulty aiDifficulty = AIDifficulty::MEDIUM;

    // =========================================================================
    // Visual & Animation State
    // =========================================================================
    float shakeTimer = 0.0f;
    float shakeIntensity = 0.0f;

    bool isAnimating = false;
    float animProgress = 0.0f;
    int animFrom = -1;
    int animTo = -1;
    int8_t animPieceCode = 0;
    sf::Sprite animSprite;

    int currentTheme = 0;
    bool useIncrement = false;

    // =========================================================================
    // Composited objects for separated concerns
    // =========================================================================
    std::unique_ptr<ChessboardRenderer> renderer;
    std::unique_ptr<ChessboardAI> ai;
    std::unique_ptr<ChessboardUI> ui;

    // =========================================================================
    // Move Validation & Pathfinding
    // =========================================================================
    bool validateMove(int s, int e, bool ignoreCheck = false);
    bool isPathClear(int s, int e);
    
    // Piece-specific validation routines
    bool validatePawn(int from, int to, int8_t piece);
    bool validateRook(int from, int to);
    bool validateBishop(int from, int to);
    bool validateKnight(int from, int to);
    bool validateQueen(int from, int to);
    bool validateKing(int from, int to);
    
    bool simulateMoveAndCheck(int s, int e, bool white);

    // =========================================================================
    // Game Logic & State Management
    // =========================================================================
    bool isKingInCheck(bool white);
    bool hasLegalMoves(bool white);
    
    void executeMoveWithEffects(int from, int to);
    void handleCapture(int target);
    void handleEnPassantCapture(int from, int to);
    void handleCastling(int from, int to);
    void updateCastlingRights(int from);
    void updateEnPassantSquare(int from, int to, int8_t piece);
    void saveSnapshot();
    void performUndo();
    void triggerShake(float intensity, float duration);
    void addMoveToLog(int from, int to, int8_t piece);
    
    // =========================================================================
    // Utility Helpers
    // =========================================================================
    bool isWhite(int8_t p) const { return p > 0; }
    bool isSameTeam(int8_t p1, int8_t p2) const {
        return (p1 != 0 && p2 != 0) && ((p1 > 0 && p2 > 0) || (p1 < 0 && p2 < 0));
    }
    int getPieceValue(int8_t piece) const;
};
