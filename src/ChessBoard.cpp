#include "ChessBoard.hpp"

#include <algorithm>
#include <climits>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "ChessBoard.hpp"
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <climits>
#include <vector>

// Piece-Square Tables for AI
const int pawnTable[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

const int knightTable[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

ChessBoard::ChessBoard() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    moveHintCircle.setRadius(15.0f);
    moveHintCircle.setOrigin(15.0f, 15.0f);
    loadAssets();
    initLabels();

    for (int i = 0; i < 64; ++i) {
        auto &s = squares.emplace_back(sf::Vector2f(Chess::PIXEL_WIDTH, Chess::PIXEL_WIDTH));
        s.setPosition((i % 8) * Chess::PIXEL_WIDTH + Chess::OFFSET_X,
                      (i / 8) * Chess::PIXEL_WIDTH + Chess::OFFSET_Y);
        s.setFillColor(((i / 8 + i % 8) % 2 == 0) ? Chess::WHITE_SQUARE : Chess::BLACK_SQUARE);
    }

    // --- Menu UI Setup ---
    pvpButton.setSize({360, 80});
    pvpButton.setFillColor(sf::Color(45, 45, 45));
    pvpButton.setOutlineThickness(2);
    pvpButton.setOutlineColor(sf::Color::White);
    pvpButton.setPosition(420, 380);

    aiButton = pvpButton;
    aiButton.setPosition(420, 490);

    pvpText.setFont(font);
    pvpText.setString("PLAYER VS PLAYER");
    pvpText.setCharacterSize(24);
    pvpText.setOrigin(pvpText.getLocalBounds().width / 2.0f, 0);
    pvpText.setPosition(600, 405);

    aiText = pvpText;
    aiText.setString("PLAYER VS COMPUTER");
    aiText.setOrigin(aiText.getLocalBounds().width / 2.0f, 0);
    aiText.setPosition(600, 515);

    menuTitle.setFont(font);
    menuTitle.setString("CHESS 2D");
    menuTitle.setCharacterSize(100);
    menuTitle.setFillColor(Chess::WINNER_GOLD);
    menuTitle.setOrigin(menuTitle.getLocalBounds().width / 2.0f, 0);
    menuTitle.setPosition(600, 150);

    resetGame();
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
    isWhiteTurn = true;
    isCheckmate = false;
    inCheck = false;
    whiteTime = 600.0f;
    blackTime = 600.0f;
    whiteScore = 0; 
    blackScore = 0;
    capturedByWhite.clear();
    capturedByBlack.clear();
    moveHistory.clear();
    wKMove = wRLMove = wRRMove = bKMove = bRLMove = bRRMove = false;
    enPassantIndex = -1;
    syncPiecesWithBoard();
}

void ChessBoard::update(float dt) {
    if (shakeTimer > 0) shakeTimer -= dt;
    else { shakeTimer = 0.0f; shakeIntensity = 0.0f; }

    if (currentState == GameState::MENU || isCheckmate) return;

    if (!isPromoting) {
        if (isWhiteTurn) whiteTime -= dt;
        else blackTime -= dt;
    }

    if (whiteTime <= 0 || blackTime <= 0) {
        whiteTime = std::max(0.0f, whiteTime);
        blackTime = std::max(0.0f, blackTime);
        isCheckmate = true;
        triggerShake(3.0f, 0.4f);
    }
}

void ChessBoard::draw(sf::RenderWindow &target) {
    if (currentState == GameState::MENU) {
        target.setView(target.getDefaultView());
        target.draw(sf::Sprite(backgroundTexture));
        target.draw(menuTitle);
        target.draw(pvpButton);
        target.draw(aiButton);
        target.draw(pvpText);
        target.draw(aiText);
        return;
    }

    // Apply Shake
    sf::View currentView = target.getDefaultView();
    if (shakeTimer > 0) {
        float rx = (static_cast<float>(std::rand() % 10) - 5.0f) * shakeIntensity;
        float ry = (static_cast<float>(std::rand() % 10) - 5.0f) * shakeIntensity;
        currentView.move(rx, ry);
    }
    target.setView(currentView);

    target.draw(sf::Sprite(backgroundTexture));
    for (auto &s : squares) target.draw(s);
    for (auto &l : labels) target.draw(l);

    // Sidebar Panels
    sf::RectangleShape topPanel(sf::Vector2f(Chess::SIDEBAR_WIDTH, 450));
    topPanel.setPosition(Chess::SIDEBAR_X, 0);
    topPanel.setFillColor(Chess::SIDEBAR_PANEL_BLACK);
    target.draw(topPanel);

    sf::RectangleShape bottomPanel(sf::Vector2f(Chess::SIDEBAR_WIDTH, 450));
    bottomPanel.setPosition(Chess::SIDEBAR_X, 450);
    bottomPanel.setFillColor(Chess::SIDEBAR_PANEL_WHITE);
    target.draw(bottomPanel);

    sf::RectangleShape turnIndicator(sf::Vector2f(8, 450));
    turnIndicator.setFillColor(Chess::TURN_COLOR);
    turnIndicator.setPosition(Chess::SIDEBAR_X, isWhiteTurn ? 450 : 0);
    if (!isCheckmate) target.draw(turnIndicator);

    sf::Text ui; ui.setFont(font);
    auto fmt = [](float t) {
        int m = (int)t / 60, s = (int)t % 60;
        std::stringstream ss;
        ss << m << ":" << std::setw(2) << std::setfill('0') << s;
        return ss.str();
    };

    // --- Black Zone (Top) ---
    bool blackWins = (isCheckmate && (isWhiteTurn || whiteTime <= 0));
    ui.setCharacterSize(55);
    ui.setFillColor(blackWins ? Chess::WINNER_GOLD : sf::Color::White);
    ui.setString(fmt(blackTime));
    ui.setOrigin(ui.getLocalBounds().width / 2, 0);
    ui.setPosition(Chess::SIDEBAR_X + 150, 40);
    target.draw(ui);

    if (blackWins) {
        ui.setCharacterSize(22); ui.setString("WINNER");
        ui.setOrigin(ui.getLocalBounds().width / 2, 0);
        ui.setPosition(Chess::SIDEBAR_X + 150, 15);
        target.draw(ui);
    }
    
    // Black Score & Captures
    ui.setOrigin(0, 0); ui.setCharacterSize(18); ui.setFillColor(sf::Color(180, 180, 180));
    ui.setString("Score: +" + std::to_string(blackScore));
    ui.setPosition(Chess::SIDEBAR_X + 25, 115);
    target.draw(ui);

    for (size_t i = 0; i < capturedByBlack.size(); ++i) {
        sf::Sprite s(textureMap[capturedByBlack[i]]);
        s.setScale(0.6f, 0.6f);
        s.setPosition(Chess::SIDEBAR_X + 25 + (i % 5) * 52, 145 + (i / 5) * 55);
        target.draw(s);
    }

    // --- White Zone (Bottom) ---
    bool whiteWins = (isCheckmate && (!isWhiteTurn || blackTime <= 0));
    ui.setCharacterSize(55);
    ui.setFillColor(whiteWins ? Chess::WINNER_GOLD : sf::Color::White);
    ui.setString(fmt(whiteTime));
    ui.setOrigin(ui.getLocalBounds().width / 2, 0);
    ui.setPosition(Chess::SIDEBAR_X + 150, 785);
    target.draw(ui);

    if (whiteWins) {
        ui.setCharacterSize(22); ui.setString("WINNER");
        ui.setOrigin(ui.getLocalBounds().width / 2, 0);
        ui.setPosition(Chess::SIDEBAR_X + 150, 855);
        target.draw(ui);
    }

    // White Score & Captures
    ui.setOrigin(0, 0); ui.setCharacterSize(18); ui.setFillColor(sf::Color(180, 180, 180));
    ui.setString("Score: +" + std::to_string(whiteScore));
    ui.setPosition(Chess::SIDEBAR_X + 25, 755);
    target.draw(ui);

    for (size_t i = 0; i < capturedByWhite.size(); ++i) {
        sf::Sprite s(textureMap[capturedByWhite[i]]);
        s.setScale(0.6f, 0.6f);
        s.setPosition(Chess::SIDEBAR_X + 25 + (i % 5) * 52, 600 + (i / 5) * 55);
        target.draw(s);
    }

    // Move Log
    ui.setCharacterSize(16); ui.setFillColor(sf::Color(150, 150, 150));
    for (size_t i = 0; i < moveHistory.size(); ++i) {
        ui.setString(moveHistory[i]);
        ui.setPosition(Chess::SIDEBAR_X + 115, 310 + (i * 20));
        target.draw(ui);
    }

    // Pieces and Selection
    if (selectedIndex != -1) {
        sf::RectangleShape s(sf::Vector2f(100, 100));
        s.setPosition(squares[selectedIndex].getPosition());
        s.setFillColor(Chess::SELECT_COLOR);
        target.draw(s);
    }
    for (const auto &hint : possibleMoves) {
        moveHintCircle.setPosition(squares[hint.index].getPosition() + sf::Vector2f(50, 50));
        moveHintCircle.setFillColor(hint.isCapture ? Chess::HINT_CAPTURE : Chess::HINT_QUIET);
        target.draw(moveHintCircle);
    }
    for (auto &p : pieceSprites) target.draw(p);

    if (isPromoting) {
        sf::RectangleShape overlay(sf::Vector2f(1200, 900));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        target.setView(target.getDefaultView());
        target.draw(overlay);
        // [Promotion Choice Rendering here]
    }
    target.setView(target.getDefaultView());
}

void ChessBoard::handleClick(const sf::Vector2i &mPos) {
    if (currentState == GameState::MENU) {
        if (pvpButton.getGlobalBounds().contains(sf::Vector2f(mPos))) {
            resetGame(); 
            setAiMode(false); 
            currentState = GameState::PLAYING;
        } else if (aiButton.getGlobalBounds().contains(sf::Vector2f(mPos))) {
            resetGame(); 
            setAiMode(true); 
            currentState = GameState::PLAYING;
        }
        return; // This return is crucial so you don't "click" the board behind the menu
    }
    
    if (currentState == GameState::MENU) {
        if (pvpButton.getGlobalBounds().contains(sf::Vector2f(mPos))) {
            resetGame(); setAiMode(false); currentState = GameState::PLAYING;
        } else if (aiButton.getGlobalBounds().contains(sf::Vector2f(mPos))) {
            resetGame(); setAiMode(true); currentState = GameState::PLAYING;
        }
        return;
    }

    if (isPromoting) {
        // [Handle Promotion Click Logic]
        return;
    }

    auto idx = getIndexFromMouse(mPos);
    if (!idx || isCheckmate) return;

    if (selectedIndex == -1) {
        if (boardState[*idx] != 0 && isWhite(boardState[*idx]) == isWhiteTurn) {
            selectedIndex = *idx; calculatePossibleMoves(selectedIndex);
        }
    } else {
        if (validateMove(selectedIndex, *idx)) {
            int8_t piece = boardState[selectedIndex];
            bool captureOccurred = (boardState[*idx] != 0 || *idx == enPassantIndex);

            addMoveToLog(selectedIndex, *idx, piece);

            // --- Capture & Scoring Logic ---
            if (boardState[*idx] != 0) {
                int val = getPieceValue(boardState[*idx]);
                if (isWhite(boardState[*idx])) {
                    capturedByBlack.push_back(boardState[*idx]);
                    blackScore += val;
                } else {
                    capturedByWhite.push_back(boardState[*idx]);
                    whiteScore += val;
                }
            }

            // En Passant Scoring
            if (std::abs(piece) == 1 && *idx == enPassantIndex) {
                int victimIdx = (piece > 0) ? *idx + 8 : *idx - 8;
                int val = getPieceValue(boardState[victimIdx]);
                if (isWhite(boardState[victimIdx])) {
                    capturedByBlack.push_back(boardState[victimIdx]);
                    blackScore += val;
                } else {
                    capturedByWhite.push_back(boardState[victimIdx]);
                    whiteScore += val;
                }
                boardState[victimIdx] = 0;
            }

            // Castling Move Handling
            if (std::abs(piece) == 7 && std::abs((*idx % 8) - (selectedIndex % 8)) == 2) {
                if (*idx == 62) { boardState[61] = boardState[63]; boardState[63] = 0; }
                if (*idx == 58) { boardState[59] = boardState[56]; boardState[56] = 0; }
                if (*idx == 6)  { boardState[5] = boardState[7];   boardState[7] = 0; }
                if (*idx == 2)  { boardState[3] = boardState[0];   boardState[0] = 0; }
            }

            enPassantIndex = -1;
            if (std::abs(piece) == 1 && std::abs((*idx / 8) - (selectedIndex / 8)) == 2)
                enPassantIndex = (piece > 0) ? *idx + 8 : *idx - 8;

            // Track piece movement for castling
            if (selectedIndex == 60) wKMove = true;
            if (selectedIndex == 63) wRRMove = true;
            if (selectedIndex == 56) wRLMove = true;
            if (selectedIndex == 4)  bKMove = true;
            if (selectedIndex == 7)  bRRMove = true;
            if (selectedIndex == 0)  bRLMove = true;

            boardState[*idx] = piece;
            boardState[selectedIndex] = 0;

            if (captureOccurred) { captureSound.play(); triggerShake(1.2f, 0.1f); }
            else moveSound.play();

            syncPiecesWithBoard();
            isWhiteTurn = !isWhiteTurn;
            inCheck = isKingInCheck(isWhiteTurn);
            
            if (inCheck && !hasLegalMoves(isWhiteTurn)) {
                isCheckmate = true; triggerShake(5.0f, 0.4f);
            } else if (inCheck) triggerShake(2.5f, 0.15f);

            selectedIndex = -1; possibleMoves.clear();
        } else if (boardState[*idx] != 0 && isSameTeam(boardState[selectedIndex], boardState[*idx])) {
            selectedIndex = *idx; calculatePossibleMoves(selectedIndex);
        } else {
            selectedIndex = -1; possibleMoves.clear();
        }
    }
}

int ChessBoard::getPieceValue(int8_t p) {
    int v = std::abs(p);
    if (v == 1) return 1;
    if (v == 3 || v == 4) return 3;
    if (v == 5) return 5;
    if (v == 9) return 9;
    return 0;
}

void ChessBoard::addMoveToLog(int s, int e, int8_t piece) {
    auto toCoord = [](int idx) {
        return std::string(1, 'a' + (idx % 8)) + std::to_string(8 - (idx / 8));
    };
    moveHistory.push_back(toCoord(s) + " " + toCoord(e));
    if (moveHistory.size() > 8) moveHistory.erase(moveHistory.begin());
}

void ChessBoard::triggerShake(float intensity, float duration) {
    shakeIntensity = intensity; shakeTimer = duration;
}

// --- AI CORE (Minimax) ---
void ChessBoard::aiMove() {
    int bestScore = isWhiteTurn ? INT_MIN : INT_MAX;
    int bestFrom = -1, bestTo = -1;

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

    for (const auto& move : moves) {
        Move m = makeMove(move.s, move.e);
        int score = minimax(4, !isWhiteTurn, INT_MIN, INT_MAX);
        undoMove(m);
        if (isWhiteTurn) { if (score > bestScore) { bestScore = score; bestFrom = move.s; bestTo = move.e; } }
        else { if (score < bestScore) { bestScore = score; bestFrom = move.s; bestTo = move.e; } }
    }

    if (bestFrom != -1) {
        handleClick(sf::Vector2i((bestTo % 8) * Chess::PIXEL_WIDTH + Chess::OFFSET_X + 50,
                                 (bestTo / 8) * Chess::PIXEL_WIDTH + Chess::OFFSET_Y + 50));
    }
}

int ChessBoard::minimax(int depth, bool isMaximizing, int alpha, int beta) {
    bool currentCheck = isKingInCheck(isMaximizing);
    if (!hasLegalMoves(isMaximizing)) return currentCheck ? (isMaximizing ? -20000 : 20000) : 0;
    if (depth == 0) return evaluateBoard();

    if (isMaximizing) {
        int maxEval = INT_MIN;
        for (int s = 0; s < 64; ++s) {
            if (boardState[s] > 0) {
                for (int e = 0; e < 64; ++e) {
                    if (validateMove(s, e, false)) {
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
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (int s = 0; s < 64; ++s) {
            if (boardState[s] < 0) {
                for (int e = 0; e < 64; ++e) {
                    if (validateMove(s, e, false)) {
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
        return minEval;
    }
}

int ChessBoard::evaluateBoard() {
    int score = 0;
    for (int i = 0; i < 64; ++i) {
        if (boardState[i] == 0) continue;
        int piece = std::abs(boardState[i]);
        int val = 0;
        int tableScore = 0;

        switch (piece) {
            case 1: val = 100; tableScore = pawnTable[boardState[i] > 0 ? i : (63 - i)]; break;
            case 3: val = 330; break;
            case 4: val = 320; tableScore = knightTable[boardState[i] > 0 ? i : (63 - i)]; break;
            case 5: val = 500; break;
            case 9: val = 900; break;
            case 7: val = 20000; break;
        }
        score += (boardState[i] > 0) ? (val + tableScore) : -(val + tableScore);
    }
    return score;
}

ChessBoard::Move ChessBoard::makeMove(int s, int e) {
    bool isEP = (e == enPassantIndex && std::abs(boardState[s]) == 1);
    Move m = {s, e, boardState[e], isEP};
    if (isEP) {
        int pawnPos = e + (boardState[s] > 0 ? 8 : -8);
        m.capturedPiece = boardState[pawnPos];
        boardState[pawnPos] = 0;
    }
    boardState[e] = boardState[s];
    boardState[s] = 0;
    return m;
}

void ChessBoard::undoMove(ChessBoard::Move m) {
    if (m.isEnPassant) {
        boardState[m.from] = boardState[m.to];
        boardState[m.to] = 0;
        int pawnPos = m.to + (boardState[m.from] > 0 ? 8 : -8);
        boardState[pawnPos] = m.capturedPiece;
    } else {
        boardState[m.from] = boardState[m.to];
        boardState[m.to] = m.capturedPiece;
    }
}

void ChessBoard::initLabels()
{
    std::string files = "abcdefgh", ranks = "87654321";
    for (int i = 0; i < 8; ++i)
    {
        sf::Text f(files.substr(i, 1), font, 24);
        f.setPosition(
            i * Chess::PIXEL_WIDTH + Chess::OFFSET_X + 40,
            8 * Chess::PIXEL_WIDTH + Chess::OFFSET_Y + 5);
        f.setFillColor(Chess::LABEL_COLOR);
        labels.push_back(f);
        sf::Text r(ranks.substr(i, 1), font, 24);
        r.setPosition(Chess::OFFSET_X - 35, i * Chess::PIXEL_WIDTH + Chess::OFFSET_Y + 30);
        r.setFillColor(Chess::LABEL_COLOR);
        labels.push_back(r);
    }
}

void ChessBoard::syncPiecesWithBoard()
{
    pieceSprites.clear();
    for (int i = 0; i < 64; ++i)
    {
        if (boardState[i] == 0)
            continue;
        sf::Sprite s(textureMap[boardState[i]]);
        s.setPosition(squares[i].getPosition());
        s.setScale(2.0f, 2.0f);
        pieceSprites.push_back(s);
    }
}

void ChessBoard::loadAssets()
{
    std::unordered_map<int8_t, std::string> fileMap = {
        {-1, "blackPawn"},
        {-3, "blackBishop"},
        {-4, "blackKnight"},
        {-5, "blackRook"},
        {-7, "blackKing"},
        {-9, "blackQueen"},
        {1, "whitePawn"},
        {3, "whiteBishop"},
        {4, "whiteKnight"},
        {5, "whiteRook"},
        {7, "whiteKing"},
        {9, "whiteQueen"}};
    for (auto const &[code, name] : fileMap)
        textureMap[code].loadFromFile("assets/images/" + name + ".png");

    backgroundTexture.loadFromFile("assets/images/landscape.jpg");
    font.loadFromFile("assets/fonts/arial.ttf");

    if (moveBuf.loadFromFile("assets/sounds/move.wav"))
        moveSound.setBuffer(moveBuf);
    if (captureBuf.loadFromFile("assets/sounds/capture.wav"))
        captureSound.setBuffer(captureBuf);
}

std::optional<int> ChessBoard::getIndexFromMouse(const sf::Vector2i &mPos)
{
    int col = (mPos.x - (int)Chess::OFFSET_X) / (int)Chess::PIXEL_WIDTH;
    int row = (mPos.y - (int)Chess::OFFSET_Y) / (int)Chess::PIXEL_WIDTH;
    if (col >= 0 && col < 8 && row >= 0 && row < 8)
        return row * 8 + col;
    return std::nullopt;
}

void ChessBoard::handleHover(const sf::Vector2i &mPos)
{
    auto idx = getIndexFromMouse(mPos);
    hoveredIndex = idx ? *idx : -1;
}

void ChessBoard::calculatePossibleMoves(int s)
{
    possibleMoves.clear();
    for (int i = 0; i < 64; ++i)
        if (validateMove(s, i, false))
            possibleMoves.push_back({i, (boardState[i] != 0 || i == enPassantIndex)});
}