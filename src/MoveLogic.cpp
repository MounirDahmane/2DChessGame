/**
 * @file MoveLogic.cpp
 * @brief Chess engine movement validation, pathfinding, and check simulation.
 * * This file implements the "Brain" of the chess engine. It separates the
 * mathematical logic of grid movement from the visual and UI components found in ChessBoard.cpp.
 */

#include "ChessBoard.hpp"

#include <cmath>

// ============================================================================
// MASTER MOVE VALIDATOR
// ============================================================================

/**
 * @brief Validates if a move from square 's' to square 'e' is legally allowed.
 * * @param s The starting square index (0-63).
 * @param e The ending square index (0-63).
 * @param ignoreCheck If true, skips simulating the move to see if it results in Check.
 * (Used heavily by the AI to prevent infinite loops).
 * @return true If the move is legal according to chess rules.
 * @return false If the move is illegal.
 */
bool ChessBoard::validateMove(int s, int e, bool ignoreCheck)
{
    int8_t p = boardState[s];

    // Cannot capture your own piece
    if (isSameTeam(p, boardState[e]))
        return false;

    bool isValid = false;

    // Switch based on the absolute value (piece type ignoring color)
    switch (std::abs(p))
    {
        case Chess::WHITE_PAWN:
            isValid = validatePawn(s, e, p);
            break;
        case Chess::WHITE_BISHOP:
            isValid = validateBishop(s, e);
            break;
        case Chess::WHITE_KNIGHT:
            isValid = validateKnight(s, e);
            break;
        case Chess::WHITE_ROOK:
            isValid = validateRook(s, e);
            break;
        case Chess::WHITE_KING:
            isValid = validateKing(s, e);
            break;
        case Chess::WHITE_QUEEN:
            isValid = validateQueen(s, e);
            break;
    }

    // If the piece move is geometrically valid, ensure it doesn't leave the King in check
    if (isValid && !ignoreCheck)
    {
        if (simulateMoveAndCheck(s, e, isWhite(p)))
        {
            isValid = false; // Move is illegal because the King would be captured
        }
    }

    return isValid;
}

// ============================================================================
// PIECE-SPECIFIC VALIDATORS
// ============================================================================

/**
 * @brief Validates pawn movement (Standard, Double push, Captures, En Passant).
 */
bool ChessBoard::validatePawn(int s, int e, int8_t p)
{
    int x1 = s % 8, y1 = s / 8;
    int x2 = e % 8, y2 = e / 8;

    // White moves UP (-1), Black moves DOWN (+1)
    int direction = (p > 0) ? -1 : 1;

    // 1. Standard Forward Move
    if (x1 == x2 && boardState[e] == Chess::EMPTY_SQUARE)
    {
        // Single push
        if (y2 == y1 + direction)
            return true;

        // 2. Double push from starting rank
        if (((p > 0 && y1 == 6) || (p < 0 && y1 == 1)) && y2 == y1 + (2 * direction))
        {
            // Must check if the square in between is empty
            if (boardState[(y1 + direction) * 8 + x1] == Chess::EMPTY_SQUARE)
                return true;
        }
    }

    // 3. Diagonal Captures (including En Passant)
    if (std::abs(x2 - x1) == 1 && y2 == y1 + direction)
    {
        // Target square must either have an enemy piece, or be the designated En Passant square
        if (boardState[e] != Chess::EMPTY_SQUARE || e == enPassantIndex)
            return true;
    }

    return false;
}

/**
 * @brief Validates King movement (1 square any direction) and Castling.
 */
bool ChessBoard::validateKing(int s, int e)
{
    int x1 = s % 8, y1 = s / 8;
    int x2 = e % 8, y2 = e / 8;
    int dx = std::abs(x1 - x2);
    int dy = std::abs(y1 - y2);

    // Standard move (1 square any direction)
    if (dx <= 1 && dy <= 1)
        return true;

    // Castling logic (Move 2 squares horizontally)
    if (y1 == y2 && dx == 2 && !inCheck)
    {
        if (isWhite(boardState[s]))
        {
            // White Kingside
            if (x2 == 6 && !whiteKingMoved && !whiteRookRightMoved && isPathClear(s, 63))
                return true;
            // White Queenside
            if (x2 == 2 && !whiteKingMoved && !whiteRookLeftMoved && isPathClear(s, 56))
                return true;
        }
        else
        {
            // Black Kingside
            if (x2 == 6 && !blackKingMoved && !blackRookRightMoved && isPathClear(s, 7))
                return true;
            // Black Queenside
            if (x2 == 2 && !blackKingMoved && !blackRookLeftMoved && isPathClear(s, 0))
                return true;
        }
    }
    return false;
}

/**
 * @brief Validates Rook movement (Horizontal or Vertical).
 */
bool ChessBoard::validateRook(int s, int e)
{
    int x1 = s % 8, y1 = s / 8;
    int x2 = e % 8, y2 = e / 8;

    // Must be in the same row or column
    if (x1 != x2 && y1 != y2)
        return false;

    return isPathClear(s, e);
}

/**
 * @brief Validates Bishop movement (Perfect diagonals only).
 */
bool ChessBoard::validateBishop(int s, int e)
{
    int x1 = s % 8, y1 = s / 8;
    int x2 = e % 8, y2 = e / 8;

    // Delta X must equal Delta Y
    if (std::abs(x1 - x2) != std::abs(y1 - y2))
        return false;

    return isPathClear(s, e);
}

/**
 * @brief Validates Knight movement (L-shape). Knights can jump over pieces.
 */
bool ChessBoard::validateKnight(int s, int e)
{
    int dx = std::abs((s % 8) - (e % 8));
    int dy = std::abs((s / 8) - (e / 8));

    return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
}

/**
 * @brief Validates Queen movement (Combination of Rook and Bishop).
 */
bool ChessBoard::validateQueen(int s, int e) { return validateRook(s, e) || validateBishop(s, e); }

// ============================================================================
// ENGINE UTILITIES (PATHFINDING & STATE CHECKING)
// ============================================================================

/**
 * @brief Checks if the path between two squares is entirely empty.
 * * Note: Does NOT check the starting or ending square, only the squares in between.
 */
bool ChessBoard::isPathClear(int s, int e)
{
    int x1 = s % 8, y1 = s / 8;
    int x2 = e % 8, y2 = e / 8;

    // Determine step direction (-1, 0, or 1)
    int dx = (x2 > x1) ? 1 : (x2 < x1 ? -1 : 0);
    int dy = (y2 > y1) ? 1 : (y2 < y1 ? -1 : 0);

    int cx = x1 + dx;
    int cy = y1 + dy;

    // Traverse the path
    while (cx != x2 || cy != y2)
    {
        if (boardState[cy * 8 + cx] != Chess::EMPTY_SQUARE)
            return false;
        cx += dx;
        cy += dy;
    }
    return true;
}

/**
 * @brief Scans the entire board to determine if the specified color's King is under attack.
 * * @param white True to check the White King, False to check the Black King.
 * @return true If the King is currently targeted by an enemy piece.
 */
bool ChessBoard::isKingInCheck(bool white)
{
    int kPos = -1;

    // 1. Locate the King
    for (int i = 0; i < 64; ++i)
    {
        if (boardState[i] == (white ? Chess::WHITE_KING : Chess::BLACK_KING))
        {
            kPos = i;
            break;
        }
    }

    if (kPos == -1)
        return false; // Should theoretically never happen unless King is captured

    // 2. See if any enemy piece can legally move to the King's square
    for (int i = 0; i < 64; ++i)
    {
        if (boardState[i] != Chess::EMPTY_SQUARE && isWhite(boardState[i]) != white)
        {
            // Pass 'true' to ignoreCheck to avoid infinite recursion
            if (validateMove(i, kPos, true))
                return true;
        }
    }
    return false;
}

/**
 * @brief Determines if the player has ANY legal moves left.
 * * Used to detect Checkmate (if in check) or Stalemate (if not in check).
 */
bool ChessBoard::hasLegalMoves(bool white)
{
    for (int s = 0; s < 64; ++s)
    {
        if (boardState[s] != Chess::EMPTY_SQUARE && isWhite(boardState[s]) == white)
        {
            for (int e = 0; e < 64; ++e)
            {
                // validateMove will internally call simulateMoveAndCheck to ensure King safety
                if (validateMove(s, e, false))
                    return true;
            }
        }
    }
    return false;
}

/**
 * @brief Temporarily executes a move, checks for Check status, and reverses it.
 * * Crucial for ensuring a player cannot make a move that puts their own King in danger.
 */
bool ChessBoard::simulateMoveAndCheck(int s, int e, bool white)
{
    Move m = makeMove(s, e);
    bool isCheck = isKingInCheck(white);
    undoMove(m);

    return isCheck;
}