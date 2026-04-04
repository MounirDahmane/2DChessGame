#pragma once

#include <array>
#include <climits>
#include <vector>
#include <cstdint>

// Forward declaration
class ChessBoard;

// ============================================================================
// AI ENGINE - SEPARATE CLASS FOR BETTER ARCHITECTURE
// ============================================================================

class ChessAIEngine {
public:
    enum class Difficulty { EASY = 2, MEDIUM = 4, HARD = 6, IMPOSSIBLE = 8 };

    explicit ChessAIEngine(Difficulty difficulty = Difficulty::MEDIUM);
    
    // Find best move using minimax with alpha-beta pruning
    std::pair<int, int> findBestMove(const ChessBoard& board);
    
    // Set difficulty level
    void setDifficulty(Difficulty diff);
    Difficulty getDifficulty() const { return difficulty; }
    
    // Get difficulty name for UI
    static const char* getDifficultyName(Difficulty diff);
    
    // Get search statistics
    int getNodesEvaluated() const { return nodesEvaluated; }
    void resetStatistics() { nodesEvaluated = 0; }

private:
    Difficulty difficulty;
    int nodesEvaluated = 0;

    // Core minimax algorithm
    int minimax(const ChessBoard& board, int depth, bool isMaximizing, 
                int alpha, int beta, bool isWhite);
    
    // Board evaluation
    int evaluateBoard(const ChessBoard& board);
    int evaluatePiece(int8_t piece, int position, bool isWhite);
    
    // Move ordering for alpha-beta pruning optimization
    struct ScoredMove {
        int from, to, score;
        bool operator<(const ScoredMove& other) const {
            return score > other.score; // Descending order
        }
    };
    std::vector<ScoredMove> generateScoredMoves(const ChessBoard& board, bool isWhite);
    
    // Utility functions
    int getPieceValue(int8_t piece) const;
    bool isWhite(int8_t piece) const { return piece > 0; }

    // Piece-square tables for position evaluation
    static constexpr int PAWN_TABLE[64] = {
        0,   0,   0,   0,   0,   0,   0,   0,
        50,  50,  50,  50,  50,  50,  50,  50,
        10,  10,  20,  30,  30,  20,  10,  10,
        5,   5,   10,  25,  25,  10,  5,   5,
        0,   0,   0,   20,  20,  0,   0,   0,
        5,   -5,  -10, 0,   0,   -10, -5,  5,
        5,   10,  10,  -20, -20, 10,  10,  5,
        0,   0,   0,   0,   0,   0,   0,   0
    };

    static constexpr int KNIGHT_TABLE[64] = {
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20, 0,   0,   0,   0,   -20, -40,
        -30, 0,   10,  15,  15,  10,  0,   -30,
        -30, 5,   15,  20,  20,  15,  5,   -30,
        -30, 0,   15,  20,  20,  15,  0,   -30,
        -30, 5,   10,  15,  15,  10,  5,   -30,
        -40, -20, 0,   5,   5,   0,   -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50
    };
};
