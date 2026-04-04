#include "ChessBoard.hpp"

bool ChessBoard::validateMove(int s, int e, bool ignoreCheck)
{
    int8_t p = boardState[s];
    if (isSameTeam(p, boardState[e])) return false;

    bool isValid = false;
    switch (std::abs(p))
    {
        case 1: isValid = validatePawn(s, e, p);   break;
        case 3: isValid = validateBishop(s, e); break;
        case 4: isValid = validateKnight(s, e); break;
        case 5: isValid = validateRook(s, e);   break;
        case 7: isValid = validateKing(s, e);   break;
        case 9: isValid = validateQueen(s, e);  break;
    }

    if (isValid && !ignoreCheck)
    {
        if (simulateMoveAndCheck(s, e, isWhite(p)))
            isValid = false;
    }
    return isValid;
}

bool ChessBoard::validatePawn(int s, int e, int8_t p)
{
    int x1 = s % 8, y1 = s / 8;
    int x2 = e % 8, y2 = e / 8;
    int d = (p > 0) ? -1 : 1;

    // Move forward
    if (x1 == x2 && boardState[e] == 0)
    {
        if (y2 == y1 + d) return true;
        // Double move from start (fixed 1D array indexing)
        if (((p > 0 && y1 == 6) || (p < 0 && y1 == 1)) && y2 == y1 + 2 * d) {
            if (boardState[(y1 + d) * 8 + x1] == 0) return true;
        }
    }
    // Capture
    if (std::abs(x2 - x1) == 1 && y2 == y1 + d)
    {
        if (boardState[e] != 0 || e == enPassantIndex) return true;
    }
    return false;
}

bool ChessBoard::validateKing(int s, int e)
{
    int x1 = s % 8, y1 = s / 8;
    int x2 = e % 8, y2 = e / 8;
    int dx = std::abs(x1 - x2);
    int dy = std::abs(y1 - y2);

    if (dx <= 1 && dy <= 1) return true;

    // Castling logic
    if (y1 == y2 && dx == 2 && !inCheck)
    {
        if (isWhite(boardState[s])) {
            if (x2 == 6 && !whiteKingMoved && !whiteRookRightMoved && isPathClear(s, 63)) return true;
            if (x2 == 2 && !whiteKingMoved && !whiteRookLeftMoved && isPathClear(s, 56)) return true;
        } else {
            if (x2 == 6 && !blackKingMoved && !blackRookRightMoved && isPathClear(s, 7)) return true;
            if (x2 == 2 && !blackKingMoved && !blackRookLeftMoved && isPathClear(s, 0)) return true;
        }
    }
    return false;
}

bool ChessBoard::validateRook(int s, int e)
{
    int x1 = s % 8, y1 = s / 8;
    int x2 = e % 8, y2 = e / 8;
    if (x1 != x2 && y1 != y2) return false;
    return isPathClear(s, e);
}

bool ChessBoard::validateBishop(int s, int e)
{
    int x1 = s % 8, y1 = s / 8;
    int x2 = e % 8, y2 = e / 8;
    if (std::abs(x1 - x2) != std::abs(y1 - y2)) return false;
    return isPathClear(s, e);
}

bool ChessBoard::validateKnight(int s, int e)
{
    int dx = std::abs((s % 8) - (e % 8));
    int dy = std::abs((s / 8) - (e / 8));
    return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
}

bool ChessBoard::validateQueen(int s, int e)
{
    return validateRook(s, e) || validateBishop(s, e);
}

bool ChessBoard::isPathClear(int s, int e)
{
    int x1 = s % 8, y1 = s / 8;
    int x2 = e % 8, y2 = e / 8;
    int dx = (x2 > x1) ? 1 : (x2 < x1 ? -1 : 0);
    int dy = (y2 > y1) ? 1 : (y2 < y1 ? -1 : 0);

    int cx = x1 + dx;
    int cy = y1 + dy;

    while (cx != x2 || cy != y2)
    {
        if (boardState[cy * 8 + cx] != 0) return false;
        cx += dx;
        cy += dy;
    }
    return true;
}

bool ChessBoard::isKingInCheck(bool white)
{
    int kPos = -1;
    for (int i = 0; i < 64; ++i)
        if (boardState[i] == (white ? 7 : -7)) { kPos = i; break; }

    if (kPos == -1) return false;
    for (int i = 0; i < 64; ++i) {
        if (boardState[i] != 0 && isWhite(boardState[i]) != white) {
            if (validateMove(i, kPos, true)) return true;
        }
    }
    return false;
}

bool ChessBoard::hasLegalMoves(bool white)
{
    for (int s = 0; s < 64; ++s) {
        if (boardState[s] != 0 && isWhite(boardState[s]) == white) {
            for (int e = 0; e < 64; ++e)
                if (validateMove(s, e, false)) return true;
        }
    }
    return false;
}

bool ChessBoard::simulateMoveAndCheck(int s, int e, bool white)
{
    Move m = makeMove(s, e);
    bool check = isKingInCheck(white);
    undoMove(m);
    return check;
}