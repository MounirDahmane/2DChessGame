#include "ChessBoard.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

ChessBoard::ChessBoard()
{
    moveHintCircle.setRadius(15.0f);
    moveHintCircle.setOrigin(15.0f, 15.0f);
    loadAssets();
    initLabels();
    for (int i = 0; i < 64; ++i)
    {
        auto &s = squares.emplace_back(sf::Vector2f(Chess::PIXEL_WIDTH, Chess::PIXEL_WIDTH));
        s.setPosition(
            (i % 8) * Chess::PIXEL_WIDTH + Chess::OFFSET_X,
            (i / 8) * Chess::PIXEL_WIDTH + Chess::OFFSET_Y);
        s.setFillColor(((i / 8 + i % 8) % 2 == 0) ? Chess::WHITE_SQUARE : Chess::BLACK_SQUARE);
    }
    syncPiecesWithBoard();
}

void ChessBoard::update(float dt)
{
    if (isCheckmate)
        return;
    if (isWhiteTurn)
        whiteTime -= dt;
    else
        blackTime -= dt;
    if (whiteTime < 0)
    {
        whiteTime = 0;
        isCheckmate = true;
    }
    if (blackTime < 0)
    {
        blackTime = 0;
        isCheckmate = true;
    }
}

int ChessBoard::getPieceValue(int8_t p)
{
    int v = std::abs(p);
    if (v == 1)
        return 1;
    if (v == 3 || v == 4)
        return 3;
    if (v == 5)
        return 5;
    if (v == 9)
        return 9;
    return 0;
}

void ChessBoard::draw(sf::RenderWindow &target)
{
    target.draw(sf::Sprite(backgroundTexture));
    for (auto &s : squares)
        target.draw(s);
    for (auto &l : labels)
        target.draw(l);

    // --- SIDEBAR PANELS (Symmetric & Opaque) ---
    sf::RectangleShape topPanel(sf::Vector2f(Chess::SIDEBAR_WIDTH, 450));
    topPanel.setPosition(Chess::SIDEBAR_X, 0);
    topPanel.setFillColor(Chess::SIDEBAR_PANEL_BLACK);
    target.draw(topPanel);

    sf::RectangleShape bottomPanel(sf::Vector2f(Chess::SIDEBAR_WIDTH, 450));
    bottomPanel.setPosition(Chess::SIDEBAR_X, 450);
    bottomPanel.setFillColor(Chess::SIDEBAR_PANEL_WHITE);
    target.draw(bottomPanel);

    // Turn Indicator (Vibrant Cyan Bar)
    sf::RectangleShape turnIndicator(sf::Vector2f(8, 450));
    turnIndicator.setFillColor(Chess::TURN_COLOR);
    if (isWhiteTurn)
        turnIndicator.setPosition(Chess::SIDEBAR_X, 450);
    else
        turnIndicator.setPosition(Chess::SIDEBAR_X, 0);
    if (!isCheckmate)
        target.draw(turnIndicator);

    sf::Text ui;
    ui.setFont(font);
    auto fmt = [](float t)
    {
        int m = (int)t / 60, s = (int)t % 60;
        std::stringstream ss;
        ss << m << ":" << std::setw(2) << std::setfill('0') << s;
        return ss.str();
    };

    // --- BLACK ZONE (TOP) ---
    bool blackWins = (isCheckmate && (isWhiteTurn || whiteTime <= 0));
    ui.setCharacterSize(55);
    ui.setFillColor(blackWins ? Chess::WINNER_GOLD : sf::Color::White);
    ui.setString(fmt(blackTime));
    ui.setOrigin(ui.getLocalBounds().width / 2, 0);
    ui.setPosition(Chess::SIDEBAR_X + 150, 40);
    target.draw(ui);

    if (blackWins)
    {
        ui.setCharacterSize(22);
        ui.setString("WINNER");
        ui.setPosition(Chess::SIDEBAR_X + 150, 15);
        target.draw(ui);
    }

    ui.setOrigin(0, 0);
    ui.setCharacterSize(18);
    ui.setFillColor(sf::Color(180, 180, 180));
    ui.setString("Score: +" + std::to_string(blackScore));
    ui.setPosition(Chess::SIDEBAR_X + 25, 115);
    target.draw(ui);

    // White pieces captured by Black
    for (size_t i = 0; i < capturedByBlack.size(); ++i)
    {
        sf::Sprite s(textureMap[capturedByBlack[i]]);
        s.setScale(0.6f, 0.6f);
        s.setPosition(Chess::SIDEBAR_X + 25 + (i % 5) * 52, 145 + (i / 5) * 55);
        target.draw(s);
    }

    // --- WHITE ZONE (BOTTOM) ---
    bool whiteWins = (isCheckmate && (!isWhiteTurn || blackTime <= 0));
    ui.setCharacterSize(55);
    ui.setFillColor(whiteWins ? Chess::WINNER_GOLD : sf::Color::White);
    ui.setString(fmt(whiteTime));
    ui.setOrigin(ui.getLocalBounds().width / 2, 0);
    ui.setPosition(Chess::SIDEBAR_X + 150, 785);
    target.draw(ui);

    if (whiteWins)
    {
        ui.setCharacterSize(22);
        ui.setString("WINNER");
        ui.setPosition(Chess::SIDEBAR_X + 150, 855);
        target.draw(ui);
    }

    ui.setOrigin(0, 0);
    ui.setCharacterSize(18);
    ui.setFillColor(sf::Color(180, 180, 180));
    ui.setString("Score: +" + std::to_string(whiteScore));
    ui.setPosition(Chess::SIDEBAR_X + 25, 755);
    target.draw(ui);

    // Black pieces captured by White
    for (size_t i = 0; i < capturedByWhite.size(); ++i)
    {
        sf::Sprite s(textureMap[capturedByWhite[i]]);
        s.setScale(0.6f, 0.6f);
        s.setPosition(Chess::SIDEBAR_X + 25 + (i % 5) * 52, 600 + (i / 5) * 55);
        target.draw(s);
    }

    // --- Better Highlighting (Rectangles) ---
    if (hoveredIndex != -1)
    {
        sf::RectangleShape h(sf::Vector2f(100, 100));
        h.setPosition(squares[hoveredIndex].getPosition());
        h.setFillColor(Chess::HOVER_COLOR);
        target.draw(h);
    }
    if (selectedIndex != -1)
    {
        sf::RectangleShape s(sf::Vector2f(100, 100));
        s.setPosition(squares[selectedIndex].getPosition());
        s.setFillColor(Chess::SELECT_COLOR);
        target.draw(s);
    }

    // Movement Hints
    for (const auto &hint : possibleMoves)
    {
        moveHintCircle.setPosition(squares[hint.index].getPosition() + sf::Vector2f(50, 50));
        moveHintCircle.setFillColor(hint.isCapture ? Chess::HINT_CAPTURE : Chess::HINT_QUIET);
        target.draw(moveHintCircle);
    }

    for (auto &p : pieceSprites)
        target.draw(p);
}

void ChessBoard::handleClick(const sf::Vector2i &mPos)
{
    auto idx = getIndexFromMouse(mPos);
    if (!idx || isCheckmate)
        return;

    if (selectedIndex == -1)
    {
        if (boardState[*idx] != 0 && isWhite(boardState[*idx]) == isWhiteTurn)
        {
            selectedIndex = *idx;
            calculatePossibleMoves(selectedIndex);
        }
    }
    else
    {
        if (validateMove(selectedIndex, *idx))
        {
            int8_t p = boardState[selectedIndex];

            // Capture Tracking
            if (boardState[*idx] != 0)
            {
                if (isWhite(boardState[*idx]))
                {
                    capturedByBlack.push_back(boardState[*idx]);
                    blackScore += getPieceValue(boardState[*idx]);
                }
                else
                {
                    capturedByWhite.push_back(boardState[*idx]);
                    whiteScore += getPieceValue(boardState[*idx]);
                }
            }

            // En Passant Execution
            if (std::abs(p) == 1 && *idx == enPassantIndex)
            {
                int victimIdx = (p > 0) ? *idx + 8 : *idx - 8;
                if (isWhite(boardState[victimIdx]))
                {
                    capturedByBlack.push_back(boardState[victimIdx]);
                    blackScore += 1;
                }
                else
                {
                    capturedByWhite.push_back(boardState[victimIdx]);
                    whiteScore += 1;
                }
                boardState[victimIdx] = 0;
            }

            // Castling Execution
            if (std::abs(p) == 7 && std::abs((*idx % 8) - (selectedIndex % 8)) == 2)
            {
                if (*idx == 62)
                {
                    boardState[61] = boardState[63];
                    boardState[63] = 0;
                }
                if (*idx == 58)
                {
                    boardState[59] = boardState[56];
                    boardState[56] = 0;
                }
                if (*idx == 6)
                {
                    boardState[5] = boardState[7];
                    boardState[7] = 0;
                }
                if (*idx == 2)
                {
                    boardState[3] = boardState[0];
                    boardState[0] = 0;
                }
            }

            // Flags Update
            enPassantIndex = -1;
            if (std::abs(p) == 1 && std::abs((*idx / 8) - (selectedIndex / 8)) == 2)
                enPassantIndex = (p > 0) ? *idx + 8 : *idx - 8;

            if (selectedIndex == 60)
                wKMove = true;
            if (selectedIndex == 63)
                wRRMove = true;
            if (selectedIndex == 56)
                wRLMove = true;
            if (selectedIndex == 4)
                bKMove = true;
            if (selectedIndex == 7)
                bRRMove = true;
            if (selectedIndex == 0)
                bRLMove = true;

            boardState[*idx] = p;
            boardState[selectedIndex] = 0;

            // Promotion (Auto-Queen)
            if (std::abs(p) == 1 && (*idx / 8 == 0 || *idx / 8 == 7))
                boardState[*idx] = (p > 0) ? 9 : -9;

            syncPiecesWithBoard();
            moveSound.play();
            isWhiteTurn = !isWhiteTurn;
            inCheck = isKingInCheck(isWhiteTurn);
            if (inCheck && !hasLegalMoves(isWhiteTurn))
                isCheckmate = true;
            selectedIndex = -1;
            possibleMoves.clear();
        }
        else if (boardState[*idx] != 0 && isSameTeam(boardState[selectedIndex], boardState[*idx]))
        {
            selectedIndex = *idx;
            calculatePossibleMoves(selectedIndex);
        }
        else
        {
            selectedIndex = -1;
            possibleMoves.clear();
        }
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
    moveBuffer.loadFromFile("assets/sounds/move.mp3");
    moveSound.setBuffer(moveBuffer);
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