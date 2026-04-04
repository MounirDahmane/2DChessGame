/**
 * @file ChessboardAI.hpp
 * @brief Logic for the chess engine's AI opponent.
 * * This file defines the ChessboardAI class, which implements a decision-making
 * engine using the Minimax algorithm with Alpha-Beta pruning and positional
 * evaluation tables.
 */

#pragma once
#include "ChessBoard.hpp"

class ChessBoard;

/**
 * @class ChessboardAI
 * @brief Handles move calculation and board evaluation for the computer opponent.
 * * The AI operates as a separate module to ensure the main ChessBoard controller
 * remains focused on game state and rule enforcement. It uses a recursive search
 * strategy to find the optimal move based on material and positional value.
 */
class ChessboardAI
{
public:
    /**
     * @brief Constructs the AI module with a reference to the active board.
     * @param boardInstance Reference to the main ChessBoard controller.
     */
    explicit ChessboardAI(ChessBoard &boardInstance);

    /**
     * @brief Orchestrates the move search and returns the best found move.
     * * This function initiates the Minimax search and is typically called
     * asynchronously by the ChessBoard to prevent UI freezing.
     * @return Move The calculated optimal move.
     */
    Move calculateAiMove();

private:
    ChessBoard &board; ///< Reference to the board state being analyzed.

    /**
     * @brief Recursive function to find the best move score using Alpha-Beta pruning.
     * @param depth Current search depth (decrements to 0).
     * @param isMaximizing True if the AI is looking for its best move, false for the opponent's
     * best response.
     * @param alpha The best score found for the maximizing player so far.
     * @param beta The best score found for the minimizing player so far.
     * @return int The evaluation score of the branch.
     */
    int minimax(int depth, bool isMaximizing, int alpha, int beta);

    /**
     * @brief Calculates the total material and positional score of the current board.
     * @return int Net score (Positive favors White, Negative favors Black).
     */
    int evaluateBoard();

    /**
     * @brief Scores an individual piece based on its type and square location.
     * @param piece The piece code (e.g., 1 for Pawn).
     * @param position 1D index (0-63) of the piece.
     * @param isWhite True if the piece belongs to the White team.
     * @return int The calculated value of the piece in its current location.
     */
    int evaluatePiece(int8_t piece, int position, bool isWhite);
};