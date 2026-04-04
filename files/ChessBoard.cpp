#include "ChessBoard.hpp"
#include "ChessboardRenderer.hpp"
#include "ChessboardAI.hpp"
#include "ChessboardUI.hpp"
#include <algorithm>
#include <climits>
#include <cmath>
#include <ctime>
#include <iostream>

ChessBoard::ChessBoard() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    shouldExit = false;
    
    // Initialize composited objects
    renderer = std::make_unique<ChessboardRenderer>(*this);
    ai = std::make_unique<ChessboardAI>(*this);
    ui = std::make_unique<ChessboardUI>(*this);
    
    // Load assets through renderer
    renderer->loadAssets();
    renderer->initLabels();
    renderer->setupMenuUI();
    renderer->setupInGameUI();
    
    // Setup board squares
    renderer->setupBoardSquares();

    resetGame();
}

ChessBoard::~ChessBoard() = default;

void ChessBoard::resetGame() {
    boardState = {
        -5, -4, -3, -9, -7, -3, -4, -5,
        -1, -1, -1, -1, -1, -1, -1, -1,
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
         1,  1,  1,  1,  1,  1,  1,  1,
         5,  4,  3,  9,  7,  3,  4,  5
    };
    renderBoardState = boardState; 
    isAiThinking = false;
    isWhiteTurn = true; inCheck = false; isCheckmate = false; isStalemate = false;
    isPromoting = false; promotionIndex = -1; enPassantIndex = -1;
    isAnimating = false;
    gameStarted = false;
    
    whiteTime = selectedTime * 60.0f; 
    blackTime = selectedTime * 60.0f; 
    
    whiteScore = 0; blackScore = 0;
    whiteKingMoved = whiteRookRightMoved = whiteRookLeftMoved = false;
    blackKingMoved = blackRookRightMoved = blackRookLeftMoved = false;
    
    capturedByWhite.clear(); capturedByBlack.clear(); moveHistory.clear(); stateHistory.clear();
    ui->clearPossibleMoves();
    ui->setSelectedIndex(-1);
}

// =================================================================================
// UPDATE & THREADING LOGIC
// =================================================================================

void ChessBoard::update(float dt) {
    if (shakeTimer > 0) shakeTimer -= dt;
    else { shakeTimer = 0.0f; shakeIntensity = 0.0f; }

    if (isAnimating) {
        animProgress += dt / 0.15f; 
        if (animProgress >= 1.0f) isAnimating = false;
        return; 
    }

    if (currentState == GameState::MENU || isCheckmate || isStalemate) return;

    if (isAiGame && !isWhiteTurn && !isPromoting) {
        if (!isAiThinking) {
            isAiThinking = true; 
            aiFuture = std::async(std::launch::async, &ChessboardAI::calculateAiMove, ai.get());
        } 
        else if (aiFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            Move bestMove = aiFuture.get();
            isAiThinking = false;
            
            if (bestMove.from != -1) {
                executeMoveWithEffects(bestMove.from, bestMove.to);
            }
        }
    }

    if (!isPromoting && !isAiThinking && gameStarted) {
        if (isWhiteTurn) whiteTime -= dt;
        else blackTime -= dt;
    }

    if (whiteTime <= 0 || blackTime <= 0) {
        whiteTime = std::max(0.0f, whiteTime);
        blackTime = std::max(0.0f, blackTime);
        isCheckmate = true;
        triggerShake(3.0f, 0.5f);
    }
}

// =================================================================================
// DRAWING LOGIC
// =================================================================================

void ChessBoard::draw(sf::RenderWindow& target) {
    renderer->draw(target);
}

// =================================================================================
// INTERACTION LOGIC
// =================================================================================

void ChessBoard::handleHover(const sf::Vector2i& mousePos) {
    ui->handleHover(mousePos);
}

void ChessBoard::handleClick(const sf::Vector2i& mousePos) {
    ui->handleClick(mousePos);
}

void ChessBoard::handleRelease(const sf::Vector2i& mousePos) {
    ui->handleRelease(mousePos);
}

// =================================================================================
// UNDO SYSTEM LOGIC
// =================================================================================

void ChessBoard::saveSnapshot() {
    StateSnapshot snap;
    snap.boardState = boardState;
    snap.isWhiteTurn = isWhiteTurn; snap.inCheck = inCheck; snap.enPassantIndex = enPassantIndex;
    snap.whiteScore = whiteScore; snap.blackScore = blackScore;
    snap.whiteKingMoved = whiteKingMoved; snap.whiteRookRightMoved = whiteRookRightMoved; snap.whiteRookLeftMoved = whiteRookLeftMoved;
    snap.blackKingMoved = blackKingMoved; snap.blackRookRightMoved = blackRookRightMoved; snap.blackRookLeftMoved = blackRookLeftMoved;
    snap.capturedByWhite = capturedByWhite; snap.capturedByBlack = capturedByBlack;
    snap.moveHistory = moveHistory;
    stateHistory.push_back(snap);
}

void ChessBoard::performUndo() {
    if (stateHistory.empty()) return;

    int pops = (isAiGame && stateHistory.size() > 1) ? 2 : 1;

    for(int i = 0; i < pops; ++i) {
        if (stateHistory.empty()) break;
        StateSnapshot snap = stateHistory.back();
        stateHistory.pop_back();

        boardState = snap.boardState;
        isWhiteTurn = snap.isWhiteTurn; inCheck = snap.inCheck; enPassantIndex = snap.enPassantIndex;
        whiteScore = snap.whiteScore; blackScore = snap.blackScore;
        whiteKingMoved = snap.whiteKingMoved; whiteRookRightMoved = snap.whiteRookRightMoved; whiteRookLeftMoved = snap.whiteRookLeftMoved;
        blackKingMoved = snap.blackKingMoved; blackRookRightMoved = snap.blackRookRightMoved; blackRookLeftMoved = snap.blackRookLeftMoved;
        capturedByWhite = snap.capturedByWhite; capturedByBlack = snap.capturedByBlack;
        moveHistory = snap.moveHistory;
    }

    renderBoardState = boardState; 
    isCheckmate = false; isStalemate = false; isPromoting = false;
    ui->setSelectedIndex(-1);
    ui->clearPossibleMoves();
}

// =================================================================================
// GAME ENGINE & MOVEMENT
// =================================================================================

void ChessBoard::executeMoveWithEffects(int from, int to) {
    saveSnapshot();
    gameStarted = true;

    int8_t piece = boardState[from];
    bool capture = (boardState[to] != 0 || to == enPassantIndex);

    animFrom = from; animTo = to; animPieceCode = piece;
    isAnimating = true; animProgress = 0.0f;
    animSprite.setTexture(renderer->getTextureForPiece(piece));
    animSprite.setScale(2.f, 2.f);

    handleCapture(to);
    handleEnPassantCapture(from, to);
    addMoveToLog(from, to, piece);
    handleCastling(from, to);
    updateCastlingRights(from);
    updateEnPassantSquare(from, to, piece);

    boardState[to] = piece;
    boardState[from] = 0;
    
    renderBoardState = boardState; 

    if (capture) { renderer->playCaptureSound(); triggerShake(1.5f, 0.15f); } 
    else renderer->playMoveSound();

    isWhiteTurn = !isWhiteTurn;
    inCheck = isKingInCheck(isWhiteTurn);

    if (std::abs(piece) == 1 && (to / 8 == 0 || to / 8 == 7)) {
        isPromoting = true; promotionIndex = to;
        renderer->setupPromotionDialog(piece);
        if (isAiGame && isWhiteTurn == false) promoteSelectedPawn(-9); 
        return; 
    }

    if (inCheck) {
        renderer->playCheckSound();
        if (!hasLegalMoves(isWhiteTurn)) { isCheckmate = true; triggerShake(5.0f, 0.4f); } 
        else triggerShake(2.5f, 0.2f);
    } else if (!hasLegalMoves(isWhiteTurn)) {
        isStalemate = true; 
    }
}

void ChessBoard::promoteSelectedPawn(int8_t promotionPiece) {
    if (promotionIndex != Chess::NO_VALID_INDEX) {
        boardState[promotionIndex] = promotionPiece;
        renderBoardState = boardState; 
        isPromoting = false;
        promotionIndex = Chess::NO_VALID_INDEX;

        inCheck = isKingInCheck(isWhiteTurn);
        if (inCheck) { 
            renderer->playCheckSound(); 
            if (!hasLegalMoves(isWhiteTurn)) { isCheckmate = true; triggerShake(5.0f, 0.4f); }
            else { triggerShake(2.5f, 0.2f); }
        }
        else if (!hasLegalMoves(isWhiteTurn)) { isStalemate = true; }
    }
}

// FIX: Strictly log the movingPiece so perfect board restoration happens during AI thinking
Move ChessBoard::makeMove(int s, int e) {
    bool isEP = (e == enPassantIndex && std::abs(boardState[s]) == 1);
    Move m = {s, e, boardState[e], boardState[s], isEP, false, 0}; 
    if (isEP) { 
        int pawnPos = e + (boardState[s] > 0 ? 8 : -8); 
        m.capturedPiece = boardState[pawnPos]; 
        boardState[pawnPos] = 0; 
    }
    boardState[e] = boardState[s]; boardState[s] = 0;
    return m;
}

// FIX: Prevent Ghost pieces by restoring precisely using movingPiece
void ChessBoard::undoMove(const Move& m) {
    if (m.isEnPassant) {
        boardState[m.from] = m.movingPiece; 
        boardState[m.to] = 0;
        int pawnPos = m.to + (m.movingPiece > 0 ? 8 : -8); boardState[pawnPos] = m.capturedPiece;
    } else {
        boardState[m.from] = m.movingPiece; 
        boardState[m.to] = m.capturedPiece;
    }
}

int ChessBoard::getPieceValue(int8_t p) const {
    int v = std::abs(p);
    if (v == 1) return 1;
    if (v == 3) return 3;
    if (v == 4) return 3;
    if (v == 5) return 5;
    if (v == 9) return 9;
    return 0;
}

void ChessBoard::addMoveToLog(int from, int to, int8_t piece) {
    auto tC = [](int i) { return std::string(1, 'a' + (i % 8)) + std::to_string(8 - (i / 8)); };
    moveHistory.push_back(tC(from) + "-" + tC(to));
    if (moveHistory.size() > 8) moveHistory.erase(moveHistory.begin());
}

void ChessBoard::triggerShake(float intensity, float duration) { 
    shakeIntensity = intensity; 
    shakeTimer = duration; 
}

void ChessBoard::handleCapture(int target) {
    if (boardState[target] != 0) {
        int8_t victim = boardState[target];
        if (isWhite(victim)) { capturedByBlack.push_back(victim); blackScore += getPieceValue(victim); } 
        else { capturedByWhite.push_back(victim); whiteScore += getPieceValue(victim); }
    }
}

void ChessBoard::handleEnPassantCapture(int from, int to) {
    if (std::abs(boardState[from]) == 1 && to == enPassantIndex) {
        int victimIdx = (boardState[from] > 0) ? to + 8 : to - 8;
        handleCapture(victimIdx); 
        boardState[victimIdx] = 0;
    }
}

void ChessBoard::handleCastling(int from, int to) {
    if (std::abs(boardState[from]) == 7 && std::abs((to % 8) - (from % 8)) == 2) {
        if (to == 62) { boardState[61] = boardState[63]; boardState[63] = 0; }
        else if (to == 58) { boardState[59] = boardState[56]; boardState[56] = 0; }
        else if (to == 6)  { boardState[5] = boardState[7]; boardState[7] = 0; }
        else if (to == 2)  { boardState[3] = boardState[0]; boardState[0] = 0; }
    }
}

void ChessBoard::updateCastlingRights(int from) {
    if (from == 60) whiteKingMoved = true;
    else if (from == 63) whiteRookRightMoved = true;
    else if (from == 56) whiteRookLeftMoved = true;
    else if (from == 4)  blackKingMoved = true;
    else if (from == 7)  blackRookRightMoved = true;
    else if (from == 0)  blackRookLeftMoved = true;
}

void ChessBoard::updateEnPassantSquare(int from, int to, int8_t piece) {
    enPassantIndex = -1;
    if (std::abs(piece) == 1 && std::abs((to / 8) - (from / 8)) == 2)
        enPassantIndex = (piece > 0) ? to + 8 : to - 8;
}

// =================================================================================
// MOVE VALIDATION LOGIC
// =================================================================================

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
