#include "ChessBoard.hpp"
#include <algorithm>
#include <climits>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream> 

static const int PAWN_TABLE[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    50,  50,  50,  50,  50,  50,  50,  50,
    10,  10,  20,  30,  30,  20,  10,  10,
    5,   5,   10,  25,  25,  10,  5,   5,
    0,   0,   0,   20,  20,  0,   0,   0,
    5,   -5,  -10, 0,   0,   -10, -5,  5,
    5,   10,  10,  -20, -20, 10,  10,  5,
    0,   0,   0,   0,   0,   0,   0,   0
};

static const int KNIGHT_TABLE[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0,   0,   0,   0,   -20, -40,
    -30, 0,   10,  15,  15,  10,  0,   -30,
    -30, 5,   15,  20,  20,  15,  5,   -30,
    -30, 0,   15,  20,  20,  15,  0,   -30,
    -30, 5,   10,  15,  15,  10,  5,   -30,
    -40, -20, 0,   5,   5,   0,   -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
};

ChessBoard::ChessBoard() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    moveHintCircle.setRadius(15.0f);
    moveHintCircle.setOrigin(15.0f, 15.0f);
    shouldExit = false;
    
    loadAssets();
    initLabels();
    setupMenuUI();
    setupInGameUI();

    for (int i = 0; i < 64; ++i) {
        auto& s = squares.emplace_back(sf::Vector2f(Chess::PIXEL_WIDTH, Chess::PIXEL_WIDTH));
        s.setPosition((i % 8) * Chess::PIXEL_WIDTH + Chess::OFFSET_X,
                      (i / 8) * Chess::PIXEL_WIDTH + Chess::OFFSET_Y);
        s.setFillColor(((i / 8 + i % 8) % 2 == 0) ? Chess::WHITE_SQUARE : Chess::BLACK_SQUARE);
    }

    resetGame();
}

void ChessBoard::setupMenuUI() {
    float centerX = Chess::WINDOW_WIDTH / 2.0f;

    pvpButton.setSize({340, 70});
    pvpButton.setFillColor(sf::Color(45, 45, 45));
    pvpButton.setOutlineThickness(2);
    pvpButton.setOutlineColor(sf::Color::White);
    
    aiButton = diffButton = timeButton = exitButton = pvpButton;
    
    pvpButton.setPosition(centerX - 170, 260);
    aiButton.setPosition(centerX - 170, 360);
    diffButton.setPosition(centerX - 170, 460);
    timeButton.setPosition(centerX - 170, 560);
    exitButton.setPosition(centerX - 170, 660);

    pvpText.setFont(font); pvpText.setString("LOCAL PVP"); pvpText.setCharacterSize(28);
    centerText(pvpText, centerX, 295);

    aiText.setFont(font); aiText.setString("VS COMPUTER"); aiText.setCharacterSize(28);
    centerText(aiText, centerX, 395);

    diffText.setFont(font); diffText.setString("AI: MEDIUM"); diffText.setCharacterSize(28);
    centerText(diffText, centerX, 495);

    timeText.setFont(font); timeText.setString("TIMER: 10 MIN"); timeText.setCharacterSize(28);
    centerText(timeText, centerX, 595);

    exitText.setFont(font); exitText.setString("EXIT GAME"); exitText.setCharacterSize(28);
    exitText.setFillColor(sf::Color(231, 76, 60)); 
    centerText(exitText, centerX, 695);

    menuTitle.setFont(font); menuTitle.setString("CHESS 2D"); menuTitle.setCharacterSize(100);
    menuTitle.setFillColor(sf::Color(241, 196, 15));
    centerText(menuTitle, centerX, 140);
}

void ChessBoard::setupInGameUI() {
    undoButton.setSize({150, 40});
    undoButton.setFillColor(sf::Color(60, 60, 60));
    
    // Changed 520 to 480 (Moved UP)
    undoButton.setPosition(Chess::SIDEBAR_X + 75, 480); 
    undoText.setFont(font); undoText.setString("UNDO MOVE");
    undoText.setCharacterSize(16);
    // Changed 540 to 500 (Moved UP to stay centered in the button)
    centerText(undoText, Chess::SIDEBAR_X + 150, 500); 

    menuButton = undoButton;
    // Changed 570 to 530 (Moved UP)
    menuButton.setPosition(Chess::SIDEBAR_X + 75, 530);
    menuBtnText = undoText; menuBtnText.setString("MAIN MENU");
    // Changed 590 to 550 (Moved UP to stay centered in the button)
    centerText(menuBtnText, Chess::SIDEBAR_X + 150, 550);

    promoBox.setSize({400, 100});
    promoBox.setFillColor(sf::Color(40, 40, 40, 240));
    promoBox.setOutlineThickness(3);
    promoBox.setOutlineColor(sf::Color(241, 196, 15));
    promoBox.setOrigin(200, 50);
    promoBox.setPosition(450, 450);

    thinkingText.setFont(font);
    thinkingText.setCharacterSize(18);
    thinkingText.setFillColor(sf::Color::Cyan);
}
void ChessBoard::centerText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    text.setPosition(x, y);
}

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
    gameStarted = false; // Prevents timer decrementing early
    
    whiteTime = selectedTime * 60.0f; 
    blackTime = selectedTime * 60.0f; 
    
    whiteScore = 0; blackScore = 0;
    whiteKingMoved = whiteRookRightMoved = whiteRookLeftMoved = false;
    blackKingMoved = blackRookRightMoved = blackRookLeftMoved = false;
    
    capturedByWhite.clear(); capturedByBlack.clear(); moveHistory.clear(); stateHistory.clear();
    possibleMoves.clear(); selectedIndex = -1;
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
            aiFuture = std::async(std::launch::async, &ChessBoard::calculateAiMove, this);
        } 
        else if (aiFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            Move bestMove = aiFuture.get();
            isAiThinking = false;
            
            if (bestMove.from != -1) {
                executeMoveWithEffects(bestMove.from, bestMove.to);
            }
        }
    }

    // FIX: Only ticks timer down if a move has been made (gameStarted = true)
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
    if (currentState == GameState::MENU) drawMenu(target);
    else drawBoard(target);
}

void ChessBoard::drawMenu(sf::RenderWindow& target) {
    target.setView(target.getDefaultView());
    target.draw(sf::Sprite(backgroundTexture));
    target.draw(menuTitle); 
    target.draw(pvpButton); target.draw(aiButton); target.draw(diffButton);
    target.draw(timeButton); target.draw(exitButton);
    
    target.draw(pvpText); target.draw(aiText); target.draw(diffText);
    target.draw(timeText); target.draw(exitText);
}

void ChessBoard::drawBoard(sf::RenderWindow& target) {
    applyScreenShake(target);
    target.draw(sf::Sprite(backgroundTexture));
    for (auto& s : squares) target.draw(s);

    if (inCheck && !isCheckmate && !isStalemate) {
        int kingIdx = -1;
        int8_t targetKing = isWhiteTurn ? Chess::WHITE_KING : Chess::BLACK_KING;
        for (int i = 0; i < 64; ++i) {
            if (renderBoardState[i] == targetKing) { kingIdx = i; break; }
        }
        if (kingIdx != -1) {
            sf::RectangleShape danger(sf::Vector2f(Chess::PIXEL_WIDTH, Chess::PIXEL_WIDTH));
            danger.setPosition(squares[kingIdx].getPosition());
            danger.setFillColor(sf::Color(255, 0, 0, 120)); 
            target.draw(danger);
        }
    }

    for (auto& l : labels) target.draw(l);
    drawSidebar(target);

    if (selectedIndex != -1) {
        sf::RectangleShape highlight(sf::Vector2f(Chess::PIXEL_WIDTH, Chess::PIXEL_WIDTH));
        highlight.setPosition(squares[selectedIndex].getPosition());
        highlight.setFillColor(Chess::SELECT_COLOR);
        target.draw(highlight);
    }

    for (const auto& hint : possibleMoves) {
        moveHintCircle.setPosition(squares[hint.index].getPosition() + sf::Vector2f(50, 50));
        moveHintCircle.setFillColor(hint.isCapture ? Chess::HINT_CAPTURE : Chess::HINT_QUIET);
        target.draw(moveHintCircle);
    }

    drawPieces(target);

    if (isAnimating) {
        sf::Vector2f start = squares[animFrom].getPosition();
        sf::Vector2f end = squares[animTo].getPosition();
        animSprite.setPosition(start + (end - start) * animProgress);
        target.draw(animSprite);
    }

    if (isPromoting) drawPromotionDialog(target);
    target.setView(target.getDefaultView());
}

void ChessBoard::drawSidebar(sf::RenderWindow& target) {
    sf::RectangleShape topPanel(sf::Vector2f(Chess::SIDEBAR_WIDTH, 450));
    topPanel.setPosition(Chess::SIDEBAR_X, 0); topPanel.setFillColor(Chess::SIDEBAR_PANEL_BLACK);
    target.draw(topPanel);

    sf::RectangleShape bottomPanel(sf::Vector2f(Chess::SIDEBAR_WIDTH, 450));
    bottomPanel.setPosition(Chess::SIDEBAR_X, 450); bottomPanel.setFillColor(Chess::SIDEBAR_PANEL_WHITE);
    target.draw(bottomPanel);

    sf::RectangleShape turnLine(sf::Vector2f(8, 450));
    turnLine.setFillColor(sf::Color::Cyan);
    turnLine.setPosition(Chess::SIDEBAR_X, isWhiteTurn ? 450 : 0);
    if (!isCheckmate && !isStalemate) target.draw(turnLine);

    sf::Text ui; ui.setFont(font);
    auto fmt = [](float t) {
        int m = (int)t / 60, s = (int)t % 60;
        std::stringstream ss; ss << m << ":" << std::setw(2) << std::setfill('0') << s;
        return ss.str();
    };

    std::string bMessage = "", wMessage = "";
    if (isStalemate) {
        bMessage = wMessage = "DRAW: STALEMATE";
    } else if (isCheckmate) {
        if (isWhiteTurn || whiteTime <= 0) bMessage = "WINNER";
        else wMessage = "WINNER";
    }

    // Black Zone
    ui.setCharacterSize(55); ui.setFillColor((bMessage != "") ? sf::Color(241, 196, 15) : sf::Color::White);
    ui.setString(fmt(blackTime)); centerText(ui, Chess::SIDEBAR_X + 150, 70);
    target.draw(ui);
    
    if (bMessage != "") {
        ui.setCharacterSize(20); ui.setString(bMessage); centerText(ui, Chess::SIDEBAR_X + 150, 25); target.draw(ui);
    }

    ui.setOrigin(0, 0); 
    ui.setCharacterSize(18); ui.setFillColor(sf::Color(180, 180, 180));
    ui.setString("Score: +" + std::to_string(blackScore));
    ui.setPosition(Chess::SIDEBAR_X + 25, 120); target.draw(ui);

    for (size_t i = 0; i < capturedByBlack.size(); ++i) {
        sf::Sprite s(textureMap[capturedByBlack[i]]); s.setScale(0.6f, 0.6f);
        s.setPosition(Chess::SIDEBAR_X + 25 + (i % 5) * 52, 150 + (i / 5) * 55); target.draw(s);
    }

    // White Zone
    ui.setCharacterSize(55); ui.setFillColor((wMessage != "") ? sf::Color(241, 196, 15) : sf::Color::White);
    ui.setString(fmt(whiteTime)); centerText(ui, Chess::SIDEBAR_X + 150, 815);
    target.draw(ui);

    if (wMessage != "") {
        ui.setCharacterSize(20); ui.setString(wMessage); centerText(ui, Chess::SIDEBAR_X + 150, 875); target.draw(ui);
    }

    ui.setOrigin(0, 0);
    ui.setCharacterSize(18); ui.setFillColor(sf::Color(100, 100, 100));
    ui.setString("Score: +" + std::to_string(whiteScore));
    ui.setPosition(Chess::SIDEBAR_X + 25, 750); target.draw(ui);

    for (size_t i = 0; i < capturedByWhite.size(); ++i) {
        sf::Sprite s(textureMap[capturedByWhite[i]]); s.setScale(0.6f, 0.6f);
        s.setPosition(Chess::SIDEBAR_X + 25 + (i % 5) * 52, 690 - (i / 5) * 55); target.draw(s);
    }

    if (!stateHistory.empty() && !isCheckmate && !isStalemate) {
        target.draw(undoButton); target.draw(undoText);
    }
    target.draw(menuButton); target.draw(menuBtnText);

    if (isAiThinking) {
        thinkingText.setString("AI IS THINKING...");
        centerText(thinkingText, Chess::SIDEBAR_X + 150, 480);
        target.draw(thinkingText);
    }

    ui.setCharacterSize(16); ui.setFillColor(sf::Color(150, 150, 150));
    for (size_t i = 0; i < moveHistory.size(); ++i) {
        ui.setString(moveHistory[i]);
        ui.setPosition(Chess::SIDEBAR_X + 120, 290 + (i * 20)); // Properly lowered move logs
        target.draw(ui);
    }
}

void ChessBoard::drawPieces(sf::RenderWindow& target) {
    for (int i = 0; i < 64; ++i) {
        if (renderBoardState[i] == 0) continue;
        if (isAnimating && i == animTo) continue; 
        if (isDragging && i == dragIndex) continue; 

        sf::Sprite sprite(textureMap[renderBoardState[i]]);
        sprite.setPosition(squares[i].getPosition());
        sprite.setScale(2.0f, 2.0f);
        target.draw(sprite);
    }

    if (isDragging && dragIndex != -1) {
        sf::Sprite dragSprite(textureMap[renderBoardState[dragIndex]]);
        dragSprite.setScale(2.0f, 2.0f);
        dragSprite.setOrigin(25.f, 25.f); 
        dragSprite.setPosition(currentMousePos);
        target.draw(dragSprite);
    }
}

void ChessBoard::drawPromotionDialog(sf::RenderWindow& target) {
    sf::RectangleShape overlay(sf::Vector2f(1200, 900));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    target.draw(overlay);
    target.draw(promoBox);
    for (auto& s : promoSprites) target.draw(s);
}

// =================================================================================
// INTERACTION LOGIC
// =================================================================================

void ChessBoard::handleClick(const sf::Vector2i& mPos) {
    if (currentState == GameState::MENU) {
        if (pvpButton.getGlobalBounds().contains(sf::Vector2f(mPos))) {
            setAiMode(false); setGameState(GameState::PLAYING); resetGame();
        } else if (aiButton.getGlobalBounds().contains(sf::Vector2f(mPos))) {
            setAiMode(true); setGameState(GameState::PLAYING); resetGame();
        } else if (diffButton.getGlobalBounds().contains(sf::Vector2f(mPos))) {
            if (aiDifficulty == AIDifficulty::EASY) { aiDifficulty = AIDifficulty::MEDIUM; diffText.setString("AI: MEDIUM"); }
            else if (aiDifficulty == AIDifficulty::MEDIUM) { aiDifficulty = AIDifficulty::HARD; diffText.setString("AI: HARD"); }
            else { aiDifficulty = AIDifficulty::EASY; diffText.setString("AI: EASY"); }
            centerText(diffText, Chess::WINDOW_WIDTH / 2.0f, 495);
        } else if (timeButton.getGlobalBounds().contains(sf::Vector2f(mPos))) {
            if (selectedTime == 1) selectedTime = 3; else if (selectedTime == 3) selectedTime = 5;
            else if (selectedTime == 5) selectedTime = 10; else selectedTime = 1;
            timeText.setString("TIMER: " + std::to_string(selectedTime) + " MIN");
            centerText(timeText, Chess::WINDOW_WIDTH / 2.0f, 595);
        } else if (exitButton.getGlobalBounds().contains(sf::Vector2f(mPos))) {
            shouldExit = true; 
        }
        return;
    }

    if (isAnimating || isAiThinking) return; 

    if (isPromoting) {
        for (size_t i = 0; i < promoSprites.size(); ++i) {
            if (promoSprites[i].getGlobalBounds().contains(sf::Vector2f(mPos))) promoteSelectedPawn(promoOptions[i]);
        }
        return;
    }

    if (menuButton.getGlobalBounds().contains(sf::Vector2f(mPos))) { setGameState(GameState::MENU); return; }
    if (isCheckmate || isStalemate) return;

    if (undoButton.getGlobalBounds().contains(sf::Vector2f(mPos))) { performUndo(); return; }

    auto idx = getIndexFromMouse(mPos);
    if (!idx) return;
    int target_idx = *idx;

    if (selectedIndex == -1) {
        if (boardState[target_idx] != 0 && isWhite(boardState[target_idx]) == isWhiteTurn) {
            selectedIndex = target_idx; 
            isDragging = true;
            dragIndex = target_idx;
            calculatePossibleMoves(selectedIndex);
        }
    } else {
        if (validateMove(selectedIndex, target_idx)) {
            executeMoveWithEffects(selectedIndex, target_idx);
            selectedIndex = -1; possibleMoves.clear();
        } else if (boardState[target_idx] != 0 && isSameTeam(boardState[selectedIndex], boardState[target_idx])) {
            selectedIndex = target_idx; 
            isDragging = true;
            dragIndex = target_idx;
            calculatePossibleMoves(selectedIndex);
        } else {
            selectedIndex = -1; possibleMoves.clear();
        }
    }
}

void ChessBoard::handleRelease(const sf::Vector2i& mPos) {
    if (!isDragging || isAiThinking) return;
    isDragging = false;

    auto idx = getIndexFromMouse(mPos);
    if (idx && *idx != dragIndex) {
        if (validateMove(dragIndex, *idx)) {
            executeMoveWithEffects(dragIndex, *idx);
            selectedIndex = -1;
            possibleMoves.clear();
        }
    }
    dragIndex = -1;
}

void ChessBoard::handleHover(const sf::Vector2i& mPos) {
    if (isAiThinking) return; 
    currentMousePos = sf::Vector2f(mPos);
    auto idx = getIndexFromMouse(mPos);
    hoveredIndex = idx ? *idx : -1;
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
        whiteKingMoved = snap.whiteKingMoved; whiteRookRightMoved = whiteRookRightMoved; whiteRookLeftMoved = whiteRookLeftMoved;
        blackKingMoved = snap.blackKingMoved; blackRookRightMoved = blackRookRightMoved; blackRookLeftMoved = snap.blackRookLeftMoved;
        capturedByWhite = snap.capturedByWhite; capturedByBlack = snap.capturedByBlack;
        moveHistory = snap.moveHistory;
    }

    renderBoardState = boardState; 
    isCheckmate = false; isStalemate = false; isPromoting = false;
    selectedIndex = -1; possibleMoves.clear();
}

// =================================================================================
// GAME ENGINE & MOVEMENT
// =================================================================================

void ChessBoard::executeMoveWithEffects(int from, int to) {
    saveSnapshot();

    gameStarted = true; // Unlocks timer ticking

    int8_t piece = boardState[from];
    bool capture = (boardState[to] != 0 || to == enPassantIndex);

    animFrom = from; animTo = to; animPieceCode = piece;
    isAnimating = true; animProgress = 0.0f;
    animSprite.setTexture(textureMap[piece]);
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

    if (capture) { captureSound.play(); triggerShake(1.5f, 0.15f); } 
    else moveSound.play();

    isWhiteTurn = !isWhiteTurn;
    inCheck = isKingInCheck(isWhiteTurn);

    if (std::abs(piece) == 1 && (to / 8 == 0 || to / 8 == 7)) {
        isPromoting = true; promotionIndex = to;
        promoSprites.clear(); promoOptions.clear();
        int8_t mult = (piece > 0) ? 1 : -1;
        promoOptions = { (int8_t)(9 * mult), (int8_t)(5 * mult), (int8_t)(3 * mult), (int8_t)(4 * mult) };
        for (int i = 0; i < 4; ++i) {
            sf::Sprite ps(textureMap[promoOptions[i]]); ps.setScale(2.f, 2.f);
            
            // FIX: Lowered position (from 380 -> 400) to fit properly inside the UI Box
            ps.setPosition(275 + (i * 90), 400); 
            
            promoSprites.push_back(ps);
        }

        if (isAiGame && isWhiteTurn == false) promoteSelectedPawn(-9); 
        return; 
    }

    if (inCheck) {
        checkSound.play(); // FIX: Reactivated check sound
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
            checkSound.play(); 
            if (!hasLegalMoves(isWhiteTurn)) { isCheckmate = true; triggerShake(5.0f, 0.4f); }
            else { triggerShake(2.5f, 0.2f); }
        }
        else if (!hasLegalMoves(isWhiteTurn)) { isStalemate = true; }
    }
}

Move ChessBoard::calculateAiMove() {
    int bestScore = isWhiteTurn ? INT_MIN : INT_MAX;
    Move bestMoveObj = {-1, -1, 0, 0, false, false, 0}; 

    struct ScoredMove { int s, e, score; };
    std::vector<ScoredMove> moves;

    for (int s = 0; s < 64; ++s) {
        if (boardState[s] != 0 && isWhite(boardState[s]) == isWhiteTurn) {
            for (int e = 0; e < 64; ++e) {
                if (validateMove(s, e, false)) {
                    int mScore = 0;
                    if (boardState[e] != 0) mScore = 100 + (std::abs(boardState[e]) - std::abs(boardState[s]));
                    moves.push_back({s, e, mScore});
                }
            }
        }
    }
    std::sort(moves.begin(), moves.end(), [](const ScoredMove& a, const ScoredMove& b) { return a.score > b.score; });

    int depth = static_cast<int>(aiDifficulty);

    for (const auto& move : moves) {
        Move m = makeMove(move.s, move.e);
        int score = minimax(depth - 1, !isWhiteTurn, INT_MIN, INT_MAX);
        undoMove(m);
        if (isWhiteTurn) { 
            if (score > bestScore) { bestScore = score; bestMoveObj = {move.s, move.e, 0, 0, false, false, 0}; } 
        } else { 
            if (score < bestScore) { bestScore = score; bestMoveObj = {move.s, move.e, 0, 0, false, false, 0}; } 
        }
    }
    
    return bestMoveObj; 
}

int ChessBoard::minimax(int depth, bool isMaximizing, int alpha, int beta) {
    if (depth <= 0) return evaluateBoard();

    int validMoves = 0;
    if (isMaximizing) {
        int maxEval = INT_MIN;
        for (int s = 0; s < 64; ++s) {
            if (boardState[s] > 0) {
                for (int e = 0; e < 64; ++e) {
                    if (validateMove(s, e, false)) {
                        validMoves++;
                        Move m = makeMove(s, e);
                        int eval = minimax(depth - 1, false, alpha, beta);
                        undoMove(m);
                        maxEval = std::max(maxEval, eval);
                        alpha = std::max(alpha, eval);
                        if (beta <= alpha) return maxEval;
                    }
                }
            }
        }
        if (validMoves == 0) return isKingInCheck(true) ? -20000 : 0;
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (int s = 0; s < 64; ++s) {
            if (boardState[s] < 0) {
                for (int e = 0; e < 64; ++e) {
                    if (validateMove(s, e, false)) {
                        validMoves++;
                        Move m = makeMove(s, e);
                        int eval = minimax(depth - 1, true, alpha, beta);
                        undoMove(m);
                        minEval = std::min(minEval, eval);
                        beta = std::min(beta, eval);
                        if (beta <= alpha) return minEval;
                    }
                }
            }
        }
        if (validMoves == 0) return isKingInCheck(false) ? 20000 : 0;
        return minEval;
    }
}

int ChessBoard::evaluateBoard() {
    int score = 0;
    for (int i = 0; i < 64; ++i) {
        if (boardState[i] == 0) continue;
        score += evaluatePiece(boardState[i], i, isWhite(boardState[i]));
    }
    return score;
}

int ChessBoard::evaluatePiece(int8_t piece, int position, bool isWhite) {
    int val = getPieceValue(piece) * 100; 
    int tableScore = 0;
    switch (std::abs(piece)) {
        case 1: tableScore = PAWN_TABLE[isWhite ? position : (63 - position)]; break;
        case 4: tableScore = KNIGHT_TABLE[isWhite ? position : (63 - position)]; break;
    }
    return isWhite ? (val + tableScore) : -(val + tableScore);
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

void ChessBoard::triggerShake(float intensity, float duration) { shakeIntensity = intensity; shakeTimer = duration; }

void ChessBoard::applyScreenShake(sf::RenderWindow& target) {
    sf::View view = target.getDefaultView();
    if (shakeTimer > 0) {
        float rx = (static_cast<float>(std::rand() % 10) - 5.0f) * shakeIntensity;
        float ry = (static_cast<float>(std::rand() % 10) - 5.0f) * shakeIntensity;
        view.move(rx, ry);
    }
    target.setView(view);
}

std::optional<int> ChessBoard::getIndexFromMouse(const sf::Vector2i& mousePos) {
    int col = (mousePos.x - static_cast<int>(Chess::OFFSET_X)) / static_cast<int>(Chess::PIXEL_WIDTH);
    int row = (mousePos.y - static_cast<int>(Chess::OFFSET_Y)) / static_cast<int>(Chess::PIXEL_WIDTH);

    if (col >= 0 && col < Chess::BOARD_SIZE && row >= 0 && row < Chess::BOARD_SIZE) {
        return row * Chess::BOARD_SIZE + col;
    }
    return std::nullopt;
}

void ChessBoard::calculatePossibleMoves(int from) {
    possibleMoves.clear();
    for (int to = 0; to < Chess::BOARD_SQUARES; ++to) {
        if (validateMove(from, to, false)) {
            possibleMoves.push_back({to, (boardState[to] != Chess::EMPTY_SQUARE || to == enPassantIndex)});
        }
    }
}

void ChessBoard::initLabels() {
    const std::string FILES = "abcdefgh";
    const std::string RANKS = "87654321";

    for (int i = 0; i < 8; ++i) {
        sf::Text fileLabel(FILES.substr(i, 1), font, 24);
        fileLabel.setPosition(i * Chess::PIXEL_WIDTH + Chess::OFFSET_X + 35, 8 * Chess::PIXEL_WIDTH + Chess::OFFSET_Y + 10);
        fileLabel.setFillColor(Chess::LABEL_COLOR);
        labels.push_back(fileLabel);

        sf::Text rankLabel(RANKS.substr(i, 1), font, 24);
        rankLabel.setPosition(Chess::OFFSET_X - 35, i * Chess::PIXEL_WIDTH + Chess::OFFSET_Y + 30);
        rankLabel.setFillColor(Chess::LABEL_COLOR);
        labels.push_back(rankLabel);
    }
}

void ChessBoard::loadAssets() {
    std::unordered_map<int8_t, std::string> fileMap = {
        {Chess::BLACK_PAWN, "blackPawn"}, {Chess::BLACK_BISHOP, "blackBishop"}, {Chess::BLACK_KNIGHT, "blackKnight"},
        {Chess::BLACK_ROOK, "blackRook"}, {Chess::BLACK_KING, "blackKing"}, {Chess::BLACK_QUEEN, "blackQueen"},
        {Chess::WHITE_PAWN, "whitePawn"}, {Chess::WHITE_BISHOP, "whiteBishop"}, {Chess::WHITE_KNIGHT, "whiteKnight"},
        {Chess::WHITE_ROOK, "whiteRook"}, {Chess::WHITE_KING, "whiteKing"}, {Chess::WHITE_QUEEN, "whiteQueen"}
    };

    for (const auto& [code, name] : fileMap) {
        std::string path = "assets/images/" + name + ".png";
        if (!textureMap[code].loadFromFile(path)) std::cerr << "ERROR: Failed to load " << path << std::endl;
    }

    if (!backgroundTexture.loadFromFile("assets/images/landscape.jpg")) std::cerr << "ERROR: Failed background" << std::endl;
    if (!font.loadFromFile("assets/fonts/arial.ttf")) std::cerr << "ERROR: Failed font" << std::endl;
    if (moveSoundBuffer.loadFromFile("assets/sounds/move.wav")) moveSound.setBuffer(moveSoundBuffer);
    if (captureSoundBuffer.loadFromFile("assets/sounds/capture.wav")) captureSound.setBuffer(captureSoundBuffer);
    
    // FIX: Re-activated Check Sound
    if (checkSoundBuffer.loadFromFile("assets/sounds/check.wav")) checkSound.setBuffer(checkSoundBuffer);
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