/**
 * @file ChessboardAI.cpp
 * @brief Implementation of the AI decision-making logic for the chess engine.
 * * This file implements the Minimax algorithm with Alpha-Beta pruning. It evaluates
 * board positions using a combination of material weight and Piece-Square Tables (PST)
 * to encourage strategic positioning (e.g., knights in the center, pawns advancing).
 */

#include "ChessboardAI.hpp"

#include <algorithm>
#include <climits>
#include <cmath>

/** * @brief Piece-Square Table for Pawns.
 * Encourages pawns to advance and occupy the center.
 */
static const int PAWN_TABLE[64] = {
    0,  0,   0,  0, 0,  0,  0,  0,   50,  50, 50, 50, 50, 50, 50, 50, 10, 10, 20, 30, 30,  20,
    10, 10,  5,  5, 10, 25, 25, 10,  5,   5,  0,  0,  0,  20, 20, 0,  0,  0,  5,  -5, -10, 0,
    0,  -10, -5, 5, 5,  10, 10, -20, -20, 10, 10, 5,  0,  0,  0,  0,  0,  0,  0,  0};

/** * @brief Piece-Square Table for Knights.
 * Penalizes knights on the edges ("Knights on the rim are dim").
 */
static const int KNIGHT_TABLE[64] = {-50, -40, -30, -30, -30, -30, -40, -50, -40, -20, 0,   0,   0,
                                     0,   -20, -40, -30, 0,   10,  15,  15,  10,  0,   -30, -30, 5,
                                     15,  20,  20,  15,  5,   -30, -30, 0,   15,  20,  20,  15,  0,
                                     -30, -30, 5,   10,  15,  15,  10,  5,   -30, -40, -20, 0,   5,
                                     5,   0,   -20, -40, -50, -40, -30, -30, -30, -30, -40, -50};

ChessboardAI::ChessboardAI(ChessBoard &boardInstance)
    : board(boardInstance)
{
}

/**
 * @brief Orchestrates the AI's move selection process.
 * * Performs an initial move generation and sorting (to improve Alpha-Beta efficiency)
 * before recursively calling Minimax.
 * * @return Move The highest-scoring legal move found.
 */
Move ChessboardAI::calculateAiMove()
{
    int bestScore = board.isWhiteTurn ? INT_MIN : INT_MAX;
    Move bestMoveObj = {-1, -1, 0, 0, false, false, 0};

    struct ScoredMove
    {
        int s, e, score;
    };
    std::vector<ScoredMove> moves;

    // Generate and pre-score moves for better pruning performance
    for (int s = 0; s < 64; ++s)
    {
        if (board.boardState[s] != 0 && board.isWhite(board.boardState[s]) == board.isWhiteTurn)
        {
            for (int e = 0; e < 64; ++e)
            {
                if (board.validateMove(s, e, false))
                {
                    int mScore = 0;
                    // Simple heuristic: prioritize captures
                    if (board.boardState[e] != 0)
                        mScore =
                            100 + (std::abs(board.boardState[e]) - std::abs(board.boardState[s]));
                    moves.push_back({s, e, mScore});
                }
            }
        }
    }

    // Sort moves descending by heuristic score
    std::sort(
        moves.begin(),
        moves.end(),
        [](const ScoredMove &a, const ScoredMove &b) { return a.score > b.score; });

    int depth = static_cast<int>(board.aiDifficulty);

    // Deep search via Minimax
    for (const auto &move : moves)
    {
        Move m = board.makeMove(move.s, move.e);
        int score = minimax(depth - 1, !board.isWhiteTurn, INT_MIN, INT_MAX);
        board.undoMove(m);

        if (board.isWhiteTurn)
        {
            if (score > bestScore)
            {
                bestScore = score;
                bestMoveObj = {move.s, move.e, 0, 0, false, false, 0};
            }
        }
        else
        {
            if (score < bestScore)
            {
                bestScore = score;
                bestMoveObj = {move.s, move.e, 0, 0, false, false, 0};
            }
        }
    }
    return bestMoveObj;
}

/**
 * @brief Recursive Minimax algorithm with Alpha-Beta pruning.
 * * @param depth Remaining search depth.
 * @param isMaximizing True if the current layer is trying to maximize the score.
 * @param alpha The best score guaranteed to the maximizer.
 * @param beta The best score guaranteed to the minimizer.
 * @return int The evaluation score of the board state.
 */
int ChessboardAI::minimax(int depth, bool isMaximizing, int alpha, int beta)
{
    if (depth <= 0)
        return evaluateBoard();

    int validMoves = 0;
    if (isMaximizing)
    {
        int maxEval = INT_MIN;
        for (int s = 0; s < 64; ++s)
        {
            if (board.boardState[s] > 0)
            {
                for (int e = 0; e < 64; ++e)
                {
                    if (board.validateMove(s, e, false))
                    {
                        validMoves++;
                        Move m = board.makeMove(s, e);
                        int eval = minimax(depth - 1, false, alpha, beta);
                        board.undoMove(m);
                        maxEval = std::max(maxEval, eval);
                        alpha = std::max(alpha, eval);
                        if (beta <= alpha)
                            return maxEval; // Beta cutoff
                    }
                }
            }
        }
        // Handle Checkmate/Stalemate
        if (validMoves == 0)
            return board.isKingInCheck(true) ? -20000 : 0;
        return maxEval;
    }
    else
    {
        int minEval = INT_MAX;
        for (int s = 0; s < 64; ++s)
        {
            if (board.boardState[s] < 0)
            {
                for (int e = 0; e < 64; ++e)
                {
                    if (board.validateMove(s, e, false))
                    {
                        validMoves++;
                        Move m = board.makeMove(s, e);
                        int eval = minimax(depth - 1, true, alpha, beta);
                        board.undoMove(m);
                        minEval = std::min(minEval, eval);
                        beta = std::min(beta, eval);
                        if (beta <= alpha)
                            return minEval; // Alpha cutoff
                    }
                }
            }
        }
        // Handle Checkmate/Stalemate
        if (validMoves == 0)
            return board.isKingInCheck(false) ? 20000 : 0;
        return minEval;
    }
}

/**
 * @brief Static evaluation function for the current board state.
 * @return int The sum of all piece evaluations.
 */
int ChessboardAI::evaluateBoard()
{
    int score = 0;
    for (int i = 0; i < 64; ++i)
    {
        if (board.boardState[i] == 0)
            continue;
        score += evaluatePiece(board.boardState[i], i, board.isWhite(board.boardState[i]));
    }
    return score;
}

/**
 * @brief Combines material value with positional value from PSTs.
 */
int ChessboardAI::evaluatePiece(int8_t piece, int position, bool isWhite)
{
    int val = board.getPieceValue(piece) * 100;
    int tableScore = 0;

    // Use PSTs for pawns and knights to improve opening/midgame play
    switch (std::abs(piece))
    {
        case 1:
            tableScore = PAWN_TABLE[isWhite ? position : (63 - position)];
            break;
        case 4:
            tableScore = KNIGHT_TABLE[isWhite ? position : (63 - position)];
            break;
    }

    return isWhite ? (val + tableScore) : -(val + tableScore);
}