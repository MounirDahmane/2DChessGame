#include "ChessAIEngine.hpp"
#include "ChessBoard.hpp"
#include <algorithm>
#include <iostream>

// ============================================================================
// CONSTRUCTOR & SETUP
// ============================================================================

ChessAIEngine::ChessAIEngine(Difficulty difficulty) 
    : difficulty(difficulty), nodesEvaluated(0) {}

void ChessAIEngine::setDifficulty(Difficulty diff) {
    difficulty = diff;
    resetStatistics();
}

const char* ChessAIEngine::getDifficultyName(Difficulty diff) {
    switch (diff) {
        case Difficulty::EASY:      return "Easy (Depth 2)";
        case Difficulty::MEDIUM:    return "Medium (Depth 4)";
        case Difficulty::HARD:      return "Hard (Depth 6)";
        case Difficulty::IMPOSSIBLE: return "Impossible (Depth 8)";
        default:                     return "Unknown";
    }
}

// ============================================================================
// MOVE FINDING
// ============================================================================

std::pair<int, int> ChessAIEngine::findBestMove(const ChessBoard& board) {
    nodesEvaluated = 0;
    
    int bestScore = board.getIsWhiteTurn() ? INT_MIN : INT_MAX;
    int bestFrom = -1;
    int bestTo = -1;

    // Generate all legal moves
    auto moves = generateScoredMoves(board, board.getIsWhiteTurn());

    if (moves.empty()) {
        return {-1, -1}; // No legal moves
    }

    // Evaluate each move
    for (const auto& move : moves) {
        ChessBoard boardCopy = board;
        boardCopy.simulateMoveForAI(move.from, move.to);

        int score = minimax(boardCopy, static_cast<int>(difficulty) - 1, 
                           !board.getIsWhiteTurn(), INT_MIN, INT_MAX, 
                           !board.getIsWhiteTurn());

        if (board.getIsWhiteTurn()) {
            if (score > bestScore) {
                bestScore = score;
                bestFrom = move.from;
                bestTo = move.to;
            }
        } else {
            if (score < bestScore) {
                bestScore = score;
                bestFrom = move.from;
                bestTo = move.to;
            }
        }
    }

    return {bestFrom, bestTo};
}

// ============================================================================
// MINIMAX WITH ALPHA-BETA PRUNING
// ============================================================================

int ChessAIEngine::minimax(const ChessBoard& board, int depth, bool isMaximizing,
                           int alpha, int beta, bool isWhite) {
    nodesEvaluated++;

    // Terminal conditions
    if (board.getIsCheckmate()) {
        return isMaximizing ? -30000 : 30000;
    }
    
    if (board.getIsStaleMate()) {
        return 0; // Draw
    }

    if (depth == 0) {
        return evaluateBoard(board);
    }

    auto moves = generateScoredMoves(board, isMaximizing);

    if (moves.empty()) {
        // No legal moves and not checkmate = stalemate
        return 0;
    }

    if (isMaximizing) {
        int maxEval = INT_MIN;
        for (const auto& move : moves) {
            ChessBoard boardCopy = board;
            boardCopy.simulateMoveForAI(move.from, move.to);
            
            int eval = minimax(boardCopy, depth - 1, false, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            
            if (beta <= alpha) break; // Beta cutoff
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (const auto& move : moves) {
            ChessBoard boardCopy = board;
            boardCopy.simulateMoveForAI(move.from, move.to);
            
            int eval = minimax(boardCopy, depth - 1, true, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            
            if (beta <= alpha) break; // Alpha cutoff
        }
        return minEval;
    }
}

// ============================================================================
// BOARD EVALUATION
// ============================================================================

int ChessAIEngine::evaluateBoard(const ChessBoard& board) {
    int score = 0;
    const auto& boardState = board.getBoardState();

    for (int i = 0; i < 64; ++i) {
        if (boardState[i] != 0) {
            int pieceScore = evaluatePiece(boardState[i], i, isWhite(boardState[i]));
            score += isWhite(boardState[i]) ? pieceScore : -pieceScore;
        }
    }

    // Add positional bonus for piece activity
    // (could be expanded with more sophisticated evaluation)
    if (board.getIsInCheck()) {
        score += board.getIsWhiteTurn() ? -50 : 50;
    }

    return score;
}

int ChessAIEngine::evaluatePiece(int8_t piece, int position, bool isWhite) {
    int8_t p = std::abs(piece);
    int baseValue = getPieceValue(piece);
    int positionBonus = 0;

    // Apply position-based bonuses from piece-square tables
    if (p == 1) { // Pawn
        positionBonus = PAWN_TABLE[isWhite ? position : (63 - position)];
    } else if (p == 4) { // Knight
        positionBonus = KNIGHT_TABLE[isWhite ? position : (63 - position)];
    }

    return baseValue + positionBonus;
}

// ============================================================================
// MOVE GENERATION & ORDERING
// ============================================================================

std::vector<ChessAIEngine::ScoredMove> ChessAIEngine::generateScoredMoves(
    const ChessBoard& board, bool isWhite) {
    
    std::vector<ScoredMove> moves;
    const auto& boardState = board.getBoardState();

    for (int from = 0; from < 64; ++from) {
        if (boardState[from] == 0) continue;
        if ((boardState[from] > 0) != isWhite) continue;

        for (int to = 0; to < 64; ++to) {
            if (board.isValidMove(from, to, false)) {
                int score = 0;

                // Prioritize captures
                if (boardState[to] != 0) {
                    int capturedValue = std::abs(boardState[to]);
                    score = 100 + capturedValue;
                }

                // Prioritize checks
                // (would need board copy to verify, so skip for now)

                moves.push_back({from, to, score});
            }
        }
    }

    // Sort by score (best moves first for better alpha-beta pruning)
    std::sort(moves.begin(), moves.end());

    return moves;
}

// ============================================================================
// UTILITY
// ============================================================================

int ChessAIEngine::getPieceValue(int8_t piece) const {
    int p = std::abs(piece);
    switch (p) {
        case 1: return 100;  // Pawn
        case 3: return 320;  // Bishop
        case 4: return 330;  // Knight
        case 5: return 500;  // Rook
        case 9: return 900;  // Queen
        case 7: return 20000; // King
        default: return 0;
    }
}
