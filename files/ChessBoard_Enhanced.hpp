#pragma once

#include "Constants_Enhanced.hpp"
#include "GameStatistics.hpp"
#include "ChessAIEngine.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <deque>
#include <map>

// ============================================================================
// MOVE & HINT STRUCTURES
// ============================================================================

struct Move {
    int from;
    int to;
    int8_t capturedPiece;
    bool isEnPassant;
    bool isCastling;
    int8_t promotionPiece;
    std::string notation;  // e.g., "e2-e4"
};

struct MoveHint {
    int index;
    bool isCapture;
};

// ============================================================================
// MAIN CHESS BOARD CLASS - ENHANCED
// ============================================================================

class ChessBoard {
public:
    // Constructor & Core Lifecycle
    ChessBoard();
    ~ChessBoard();
    void update(float dt);
    void draw(sf::RenderWindow& target);
    void handleHover(const sf::Vector2i& mousePos);
    void handleClick(const sf::Vector2i& mousePos);

    // Game Control
    void resetGame();
    void aiMove();
    void promoteSelectedPawn(int8_t promotionPiece);
    void undoMove();
    void redoMove();
    void pauseGame();
    void resumeGame();

    // Getters
    bool getIsAiGame() const { return isAiGame; }
    bool getIsWhiteTurn() const { return isWhiteTurn; }
    bool getIsCheckmate() const { return isCheckmate; }
    bool getIsStaleMate() const { return isStaleMate; }
    bool getIsPromoting() const { return isPromoting; }
    bool getIsInCheck() const { return inCheck; }
    bool getIsPaused() const { return isPaused; }
    Chess::GameState getState() const { return currentState; }
    float getWhiteTime() const { return whiteTime; }
    float getBlackTime() const { return blackTime; }
    int getWhiteScore() const { return whiteScore; }
    int getBlackScore() const { return blackScore; }
    
    // AI & Difficulty
    Chess::Difficulty getAIDifficulty() const { return aiDifficulty; }
    void setAIDifficulty(Chess::Difficulty diff);
    const char* getAIDifficultyName() const;
    
    // Statistics
    const GameStatistics& getGameStats() const { return gameStats; }
    void finalizeGameStats();

    // Board access for AI
    const std::array<int8_t, Chess::BOARD_SQUARES>& getBoardState() const { return boardState; }
    bool isValidMove(int from, int to, bool ignoreCheck = false) const;
    void simulateMoveForAI(int from, int to);
    
    // Setters
    void setAiMode(bool mode) { isAiGame = mode; }
    void setGameState(Chess::GameState state) { currentState = state; }
    void setLocalPlayerName(const std::string& name);
    void setOpponentName(const std::string& name);

private:
    // ========================================================================
    // GAME STATE SECTION
    // ========================================================================

    Chess::GameState currentState = Chess::GameState::MENU;
    bool isAiGame = false;
    bool isWhiteTurn = true;
    int selectedIndex = Chess::NO_VALID_INDEX;
    int hoveredIndex = Chess::NO_VALID_INDEX;

    // Board representation
    std::array<int8_t, Chess::BOARD_SQUARES> boardState;

    // Game state flags
    bool inCheck = false;
    bool isCheckmate = false;
    bool isStaleMate = false;
    int enPassantIndex = Chess::NO_VALID_INDEX;
    bool isPromoting = false;
    int promotionIndex = Chess::NO_VALID_INDEX;
    bool isPaused = false;

    // Castling rights tracking
    bool whiteKingMoved = false;
    bool whiteRookRightMoved = false;
    bool whiteRookLeftMoved = false;
    bool blackKingMoved = false;
    bool blackRookRightMoved = false;
    bool blackRookLeftMoved = false;

    // Timers and Scoring
    float whiteTime = Chess::DEFAULT_TIME_LIMIT;
    float blackTime = Chess::DEFAULT_TIME_LIMIT;
    int whiteScore = 0;
    int blackScore = 0;

    // Threefold Repetition & 50-Move Rule
    int movesSinceLastCaptureOrPawnMove = 0;
    std::deque<std::array<int8_t, Chess::BOARD_SQUARES>> boardHistory;
    std::map<std::string, int> boardPositionCount;  // For threefold repetition

    // History Tracking
    std::vector<int8_t> capturedByWhite;
    std::vector<int8_t> capturedByBlack;
    std::vector<Move> moveHistory;
    std::vector<Move> undoneHistory;  // For redo functionality
    std::vector<std::string> moveNotationHistory;

    // AI & Difficulty
    Chess::Difficulty aiDifficulty = Chess::Difficulty::MEDIUM;
    std::unique_ptr<ChessAIEngine> aiEngine;
    float aiMoveTimer = 0.0f;

    // Game Statistics
    GameStatistics gameStats;

    // Visual Effects
    float shakeTimer = 0.0f;
    float shakeIntensity = 0.0f;

    // ========================================================================
    // RENDERING & ASSETS
    // ========================================================================

    // Board UI
    std::vector<sf::RectangleShape> squares;
    std::vector<sf::Sprite> pieceSprites;
    std::vector<sf::Text> labels;

    // Menu UI & Buttons
    std::vector<sf::RectangleShape> menuButtons;
    std::vector<sf::Text> menuButtonTexts;
    sf::Text menuTitle;
    
    // Difficulty selection UI
    std::vector<sf::RectangleShape> difficultyButtons;
    std::vector<sf::Text> difficultyTexts;
    
    // Promotion dialog UI
    std::vector<sf::RectangleShape> promotionButtons;
    std::vector<sf::Text> promotionTexts;

    // Assets
    sf::Font font;
    sf::Texture backgroundTexture;
    std::unordered_map<int8_t, sf::Texture> textureMap;

    // Audio
    sf::SoundBuffer moveSoundBuffer;
    sf::SoundBuffer captureSoundBuffer;
    sf::SoundBuffer checkSoundBuffer;
    sf::Sound moveSound;
    sf::Sound captureSound;
    sf::Sound checkSound;

    // Move hints
    sf::CircleShape moveHintCircle;
    std::vector<MoveHint> possibleMoves;

    // ========================================================================
    // PRIVATE METHODS - BOARD OPERATIONS
    // ========================================================================

    void syncPiecesWithBoard();
    void calculatePossibleMoves(int startIdx);
    std::optional<int> getIndexFromMouse(const sf::Vector2i& mPos);
    std::string getBoardPositionHash() const;

    // ========================================================================
    // PRIVATE METHODS - MOVE VALIDATION
    // ========================================================================

    bool validateMove(int from, int to, bool ignoreCheck = false);
    bool simulateMoveAndCheck(int from, int to, bool white);
    bool isKingInCheck(bool white);
    bool hasLegalMoves(bool white);
    bool isPathClear(int from, int to);
    bool isThreefoldRepetition() const;
    bool isFiftyMoveRuleApplicable() const;

    // Piece-specific validation
    bool validatePawn(int from, int to, int8_t piece);
    bool validateRook(int from, int to);
    bool validateBishop(int from, int to);
    bool validateKnight(int from, int to);
    bool validateQueen(int from, int to);
    bool validateKing(int from, int to);

    // ========================================================================
    // PRIVATE METHODS - MOVE EXECUTION
    // ========================================================================

    void executeMoveWithEffects(int from, int to);
    void handleCapture(int target);
    void handleEnPassantCapture(int from, int to);
    void handleCastling(int from, int to);
    void updateCastlingRights(int from);
    void updateEnPassantSquare(int from, int to, int8_t piece);
    void updateBoardHistory();
    void updateMoveCounters(int from, int to);

    // ========================================================================
    // PRIVATE METHODS - RENDERING
    // ========================================================================

    void loadAssets();
    void initLabels();
    void setupMenuUI();
    void setupDifficultyUI();
    void setupPromotionUI();
    
    void drawMenu(sf::RenderWindow& target);
    void drawDifficultySelection(sf::RenderWindow& target);
    void drawBoard(sf::RenderWindow& target);
    void drawSidebar(sf::RenderWindow& target);
    void drawPieces(sf::RenderWindow& target);
    void drawPromotionDialog(sf::RenderWindow& target);
    void drawGameStats(sf::RenderWindow& target);
    void applyScreenShake(sf::RenderWindow& target);
    
    void centerText(sf::Text& text, float x, float y);

    // ========================================================================
    // PRIVATE METHODS - UTILITY
    // ========================================================================

    bool isWhite(int8_t piece) const { return piece > 0; }
    bool isSameTeam(int8_t p1, int8_t p2) const {
        return (p1 != 0 && p2 != 0) && ((p1 > 0 && p2 > 0) || (p1 < 0 && p2 < 0));
    }

    int getPieceValue(int8_t piece) const;
    void addMoveToLog(int from, int to, int8_t piece);
    void triggerShake(float intensity, float duration);
    
    // Stalemate & Special rules
    void checkForGameEnd();
    void updateGameStats();
};
