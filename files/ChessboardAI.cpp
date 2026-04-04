#include "ChessboardAI.hpp"
#include "ChessBoard.hpp"
#include <algorithm>
#include <climits>

ChessboardAI::ChessboardAI(ChessBoard& board) : board(board) {}

Move ChessboardAI::calculateAiMove() {
    int bestScore = board.getIsWhiteTurn() ? INT_MIN : INT_MAX;
    Move bestMoveObj = {-1, -1, 0, 0, false, false, 0}; 

    struct ScoredMove { int s, e, score; };
    std::vector<ScoredMove> moves;

    const auto& boardState = board.getBoardState();
    for (int s = 0; s < 64; ++s) {
        if (boardState[s] != 0 && board.isWhite(boardState[s]) == board.getIsWhiteTurn()) {
            for (int e = 0; e < 64; ++e) {
                if (board.validateMove(s, e, false)) {
                    int mScore = 0;
                    if (boardState[e] != 0) mScore = 100 + (std::abs(boardState[e]) - std::abs(boardState[s]));
                    moves.push_back({s, e, mScore});
                }
            }
        }
    }
    std::sort(moves.begin(), moves.end(), [](const ScoredMove& a, const ScoredMove& b) { return a.score > b.score; });

    int depth = static_cast<int>(board.aiDifficulty);

    for (const auto& move : moves) {
        Move m = board.makeMove(move.s, move.e);
        int score = minimax(depth - 1, !board.getIsWhiteTurn(), INT_MIN, INT_MAX);
        board.undoMove(m);
        if (board.getIsWhiteTurn()) { 
            if (score > bestScore) { bestScore = score; bestMoveObj = {move.s, move.e, 0, 0, false, false, 0}; } 
        } else { 
            if (score < bestScore) { bestScore = score; bestMoveObj = {move.s, move.e, 0, 0, false, false, 0}; } 
        }
    }
    
    return bestMoveObj; 
}

int ChessboardAI::minimax(int depth, bool isMaximizing, int alpha, int beta) {
    if (depth <= 0) return evaluateBoard();

    int validMoves = 0;
    const auto& boardState = board.getBoardState();
    
    if (isMaximizing) {
        int maxEval = INT_MIN;
        for (int s = 0; s < 64; ++s) {
            if (boardState[s] > 0) {
                for (int e = 0; e < 64; ++e) {
                    if (board.validateMove(s, e, false)) {
                        validMoves++;
                        Move m = board.makeMove(s, e);
                        int eval = minimax(depth - 1, false, alpha, beta);
                        board.undoMove(m);
                        maxEval = std::max(maxEval, eval);
                        alpha = std::max(alpha, eval);
                        if (beta <= alpha) return maxEval;
                    }
                }
            }
        }
        if (validMoves == 0) return board.isKingInCheck(true) ? -20000 : 0;
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (int s = 0; s < 64; ++s) {
            if (boardState[s] < 0) {
                for (int e = 0; e < 64; ++e) {
                    if (board.validateMove(s, e, false)) {
                        validMoves++;
                        Move m = board.makeMove(s, e);
                        int eval = minimax(depth - 1, true, alpha, beta);
                        board.undoMove(m);
                        minEval = std::min(minEval, eval);
                        beta = std::min(beta, eval);
                        if (beta <= alpha) return minEval;
                    }
                }
            }
        }
        if (validMoves == 0) return board.isKingInCheck(false) ? 20000 : 0;
        return minEval;
    }
}

int ChessboardAI::evaluateBoard() const {
    int score = 0;
    const auto& boardState = board.getBoardState();
    for (int i = 0; i < 64; ++i) {
        if (boardState[i] == 0) continue;
        score += evaluatePiece(boardState[i], i, board.isWhite(boardState[i]));
    }
    return score;
}

int ChessboardAI::evaluatePiece(int8_t piece, int position, bool isWhite) const {
    int val = board.getPieceValue(piece) * 100; 
    int tableScore = 0;
    switch (std::abs(piece)) {
        case 1: tableScore = PAWN_TABLE[isWhite ? position : (63 - position)]; break;
        case 4: tableScore = KNIGHT_TABLE[isWhite ? position : (63 - position)]; break;
    }
    return isWhite ? (val + tableScore) : -(val + tableScore);
}
