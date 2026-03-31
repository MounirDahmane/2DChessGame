#include "ChessBoard.hpp"

bool ChessBoard::validateMove(int s, int e, bool ignoreCheck)
{
    int8_t p = boardState[s];
    if (isSameTeam(p, boardState[e]))
        return false;
    bool v = false;
    switch (std::abs(p))
    {
        case 1:
            v = valPawn(s, e, p);
            break;
        case 3:
            v = valBishop(s, e);
            break;
        case 4:
            v = valKnight(s, e);
            break;
        case 5:
            v = valRook(s, e);
            break;
        case 7:
            v = valKing(s, e);
            break;
        case 9:
            v = valQueen(s, e);
            break;
    }
    if (v && !ignoreCheck)
        if (simulateMoveAndCheck(s, e, isWhite(p)))
            v = false;
    return v;
}

bool ChessBoard::valPawn(int s, int e, int8_t p)
{
    int x1 = s % 8, y1 = s / 8, x2 = e % 8, y2 = e / 8, d = (p > 0) ? -1 : 1;
    if (x1 == x2 && boardState[e] == 0)
    {
        if (y2 == y1 + d)
            return true;
        if (((p > 0 && y1 == 6) || (p < 0 && y1 == 1)) && y2 == y1 + 2 * d &&
            boardState[x1 + 8 * (y1 + d)] == 0)
            return true;
    }
    if (std::abs(x1 - x2) == 1 && y2 == y1 + d && (boardState[e] != 0 || e == enPassantIndex))
        return true;
    return false;
}

bool ChessBoard::valKing(int s, int e)
{
    int dx = std::abs(s % 8 - e % 8), dy = std::abs(s / 8 - e / 8);
    if (dx <= 1 && dy <= 1)
        return true;
    if (dy == 0 && dx == 2 && !inCheck)
    {
        if (isWhiteTurn)
        {
            if (!wKMove && ((e == 62 && !wRRMove && isPathClear(60, 63)) ||
                            (e == 58 && !wRLMove && isPathClear(60, 56))))
                return true;
        }
        else
        {
            if (!bKMove && ((e == 6 && !bRRMove && isPathClear(4, 7)) ||
                            (e == 2 && !bRLMove && isPathClear(4, 0))))
                return true;
        }
    }
    return false;
}

bool ChessBoard::simulateMoveAndCheck(int s, int e, bool white)
{
    int8_t sP = boardState[s], eP = boardState[e];
    boardState[e] = boardState[s];
    boardState[s] = 0;
    bool check = isKingInCheck(white);

    boardState[s] = sP;
    boardState[e] = eP;
    return check;
}

bool ChessBoard::isKingInCheck(bool white)
{
    int kPos = -1;
    for (int i = 0; i < 64; ++i)
        if (boardState[i] == (white ? 7 : -7))
            kPos = i;
    if (kPos == -1)
        return false;
    for (int i = 0; i < 64; ++i)
    {
        if (boardState[i] != 0 && isWhite(boardState[i]) != white)
        {
            if (validateMove(i, kPos, true))
                return true;
        }
    }
    return false;
}

bool ChessBoard::hasLegalMoves(bool white)
{
    for (int s = 0; s < 64; ++s)
    {
        if (boardState[s] != 0 && isWhite(boardState[s]) == white)
        {
            for (int e = 0; e < 64; ++e)
                if (validateMove(s, e, false))
                    return true;
        }
    }
    return false;
}

bool ChessBoard::isPathClear(int s, int e)
{
    int x1 = s % 8, y1 = s / 8, x2 = e % 8, y2 = e / 8;
    int dx = (x2 > x1) ? 1 : (x2 < x1 ? -1 : 0), dy = (y2 > y1) ? 1 : (y2 < y1 ? -1 : 0);
    int cx = x1 + dx, cy = y1 + dy;
    while (cx != x2 || cy != y2)
    {
        if (boardState[cy * 8 + cx] != 0)
            return false;
        cx += dx;
        cy += dy;
    }
    return true;
}

bool ChessBoard::valRook(int s, int e)
{
    return (s % 8 == e % 8 || s / 8 == e / 8) && isPathClear(s, e);
}
bool ChessBoard::valBishop(int s, int e)
{
    return std::abs(s % 8 - e % 8) == std::abs(s / 8 - e / 8) && isPathClear(s, e);
}
bool ChessBoard::valKnight(int s, int e)
{
    int dx = std::abs(s % 8 - e % 8), dy = std::abs(s / 8 - e / 8);
    return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
}
bool ChessBoard::valQueen(int s, int e) { return valRook(s, e) || valBishop(s, e); }