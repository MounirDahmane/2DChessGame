#include "ChessboardRenderer.hpp"
#include "ChessBoard.hpp"
#include "ChessboardUI.hpp"
#include <iomanip>
#include <sstream>
#include <iostream>

ChessboardRenderer::ChessboardRenderer(ChessBoard& board) : board(board) {
    moveHintCircle.setRadius(15.0f);
    moveHintCircle.setOrigin(15.0f, 15.0f);
}

void ChessboardRenderer::loadAssets() {
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
    if (checkSoundBuffer.loadFromFile("assets/sounds/check.wav")) checkSound.setBuffer(checkSoundBuffer);
}

void ChessboardRenderer::setupMenuUI() {
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

void ChessboardRenderer::setupInGameUI() {
    undoButton.setSize({150, 40});
    undoButton.setFillColor(sf::Color(60, 60, 60));
    undoButton.setPosition(Chess::SIDEBAR_X + 75, 480); 
    undoText.setFont(font); undoText.setString("UNDO MOVE");
    undoText.setCharacterSize(16);
    centerText(undoText, Chess::SIDEBAR_X + 150, 500); 

    menuButton = undoButton;
    menuButton.setPosition(Chess::SIDEBAR_X + 75, 530);
    menuBtnText = undoText; menuBtnText.setString("MAIN MENU");
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

void ChessboardRenderer::setupBoardSquares() {
    for (int i = 0; i < 64; ++i) {
        auto& s = squares.emplace_back(sf::Vector2f(Chess::PIXEL_WIDTH, Chess::PIXEL_WIDTH));
        s.setPosition((i % 8) * Chess::PIXEL_WIDTH + Chess::OFFSET_X,
                      (i / 8) * Chess::PIXEL_WIDTH + Chess::OFFSET_Y);
        s.setFillColor(((i / 8 + i % 8) % 2 == 0) ? Chess::WHITE_SQUARE : Chess::BLACK_SQUARE);
    }
}

void ChessboardRenderer::initLabels() {
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

void ChessboardRenderer::setupPromotionDialog(int8_t piece) {
    promoSprites.clear(); 
    promoOptions.clear();
    int8_t mult = (piece > 0) ? 1 : -1;
    promoOptions = { (int8_t)(9 * mult), (int8_t)(5 * mult), (int8_t)(3 * mult), (int8_t)(4 * mult) };
    for (int i = 0; i < 4; ++i) {
        sf::Sprite ps(textureMap[promoOptions[i]]); ps.setScale(2.f, 2.f);
        ps.setPosition(275 + (i * 90), 400); 
        promoSprites.push_back(ps);
    }
}

void ChessboardRenderer::centerText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    text.setPosition(x, y);
}

void ChessboardRenderer::draw(sf::RenderWindow& target) {
    if (board.getState() == ChessBoard::GameState::MENU) drawMenu(target);
    else drawBoard(target);
}

void ChessboardRenderer::drawMenu(sf::RenderWindow& target) {
    target.setView(target.getDefaultView());
    target.draw(sf::Sprite(backgroundTexture));
    target.draw(menuTitle); 
    target.draw(pvpButton); target.draw(aiButton); target.draw(diffButton);
    target.draw(timeButton); target.draw(exitButton);
    
    target.draw(pvpText); target.draw(aiText); target.draw(diffText);
    target.draw(timeText); target.draw(exitText);
}

void ChessboardRenderer::drawBoard(sf::RenderWindow& target) {
    applyScreenShake(target);
    target.draw(sf::Sprite(backgroundTexture));
    for (auto& s : squares) target.draw(s);

    if (board.getIsInCheck() && !board.getIsCheckmate()) {
        int kingIdx = -1;
        int8_t targetKing = board.getIsWhiteTurn() ? Chess::WHITE_KING : Chess::BLACK_KING;
        for (int i = 0; i < 64; ++i) {
            if (board.getRenderBoardState()[i] == targetKing) { kingIdx = i; break; }
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

    int selectedIdx = board.ui->getSelectedIndex();
    if (selectedIdx != -1) {
        sf::RectangleShape highlight(sf::Vector2f(Chess::PIXEL_WIDTH, Chess::PIXEL_WIDTH));
        highlight.setPosition(squares[selectedIdx].getPosition());
        highlight.setFillColor(Chess::SELECT_COLOR);
        target.draw(highlight);
    }

    const auto& possibleMoves = board.ui->getPossibleMoves();
    for (const auto& hint : possibleMoves) {
        moveHintCircle.setPosition(squares[hint.index].getPosition() + sf::Vector2f(50, 50));
        moveHintCircle.setFillColor(hint.isCapture ? Chess::HINT_CAPTURE : Chess::HINT_QUIET);
        target.draw(moveHintCircle);
    }

    drawPieces(target);

    if (board.isAnimating) {
        sf::Vector2f start = squares[board.animFrom].getPosition();
        sf::Vector2f end = squares[board.animTo].getPosition();
        board.animSprite.setPosition(start + (end - start) * board.animProgress);
        target.draw(board.animSprite);
    }

    if (board.getIsPromoting()) drawPromotionDialog(target);
    target.setView(target.getDefaultView());
}

void ChessboardRenderer::drawSidebar(sf::RenderWindow& target) {
    sf::RectangleShape topPanel(sf::Vector2f(Chess::SIDEBAR_WIDTH, 450));
    topPanel.setPosition(Chess::SIDEBAR_X, 0); topPanel.setFillColor(Chess::SIDEBAR_PANEL_BLACK);
    target.draw(topPanel);

    sf::RectangleShape bottomPanel(sf::Vector2f(Chess::SIDEBAR_WIDTH, 450));
    bottomPanel.setPosition(Chess::SIDEBAR_X, 450); bottomPanel.setFillColor(Chess::SIDEBAR_PANEL_WHITE);
    target.draw(bottomPanel);

    sf::RectangleShape turnLine(sf::Vector2f(8, 450));
    turnLine.setFillColor(sf::Color::Cyan);
    turnLine.setPosition(Chess::SIDEBAR_X, board.getIsWhiteTurn() ? 450 : 0);
    if (!board.getIsCheckmate()) target.draw(turnLine);

    sf::Text ui; ui.setFont(font);
    auto fmt = [](float t) {
        int m = (int)t / 60, s = (int)t % 60;
        std::stringstream ss; ss << m << ":" << std::setw(2) << std::setfill('0') << s;
        return ss.str();
    };

    std::string bMessage = "", wMessage = "";
    if (board.isStalemate) {
        bMessage = wMessage = "DRAW: STALEMATE";
    } else if (board.getIsCheckmate()) {
        if (board.getIsWhiteTurn() || board.blackTime <= 0) bMessage = "WINNER";
        else wMessage = "WINNER";
    }

    // Black Zone
    ui.setCharacterSize(55); ui.setFillColor((bMessage != "") ? sf::Color(241, 196, 15) : sf::Color::White);
    ui.setString(fmt(board.blackTime)); centerText(ui, Chess::SIDEBAR_X + 150, 70);
    target.draw(ui);
    
    if (bMessage != "") {
        ui.setCharacterSize(20); ui.setString(bMessage); centerText(ui, Chess::SIDEBAR_X + 150, 25); target.draw(ui);
    }

    ui.setOrigin(0, 0); 
    ui.setCharacterSize(18); ui.setFillColor(sf::Color(180, 180, 180));
    ui.setString("Score: +" + std::to_string(board.blackScore));
    ui.setPosition(Chess::SIDEBAR_X + 25, 120); target.draw(ui);

    for (size_t i = 0; i < board.capturedByBlack.size(); ++i) {
        sf::Sprite s(textureMap[board.capturedByBlack[i]]); s.setScale(0.6f, 0.6f);
        s.setPosition(Chess::SIDEBAR_X + 25 + (i % 5) * 52, 150 + (i / 5) * 55); target.draw(s);
    }

    // White Zone
    ui.setCharacterSize(55); ui.setFillColor((wMessage != "") ? sf::Color(241, 196, 15) : sf::Color::White);
    ui.setString(fmt(board.whiteTime)); centerText(ui, Chess::SIDEBAR_X + 150, 815);
    target.draw(ui);

    if (wMessage != "") {
        ui.setCharacterSize(20); ui.setString(wMessage); centerText(ui, Chess::SIDEBAR_X + 150, 875); target.draw(ui);
    }

    ui.setOrigin(0, 0);
    ui.setCharacterSize(18); ui.setFillColor(sf::Color(100, 100, 100));
    ui.setString("Score: +" + std::to_string(board.whiteScore));
    ui.setPosition(Chess::SIDEBAR_X + 25, 750); target.draw(ui);

    for (size_t i = 0; i < board.capturedByWhite.size(); ++i) {
        sf::Sprite s(textureMap[board.capturedByWhite[i]]); s.setScale(0.6f, 0.6f);
        s.setPosition(Chess::SIDEBAR_X + 25 + (i % 5) * 52, 690 - (i / 5) * 55); target.draw(s);
    }

    if (!board.stateHistory.empty() && !board.getIsCheckmate()) {
        target.draw(undoButton); target.draw(undoText);
    }
    target.draw(menuButton); target.draw(menuBtnText);

    if (board.isAiThinking) {
        thinkingText.setString("AI IS THINKING...");
        centerText(thinkingText, Chess::SIDEBAR_X + 150, 480);
        target.draw(thinkingText);
    }

    ui.setCharacterSize(16); ui.setFillColor(sf::Color(150, 150, 150));
    for (size_t i = 0; i < board.moveHistory.size(); ++i) {
        ui.setString(board.moveHistory[i]);
        ui.setPosition(Chess::SIDEBAR_X + 120, 290 + (i * 20));
        target.draw(ui);
    }
}

void ChessboardRenderer::drawPieces(sf::RenderWindow& target) {
    for (int i = 0; i < 64; ++i) {
        if (board.getRenderBoardState()[i] == 0) continue;
        if (board.isAnimating && i == board.animTo) continue; 
        if (board.ui->isDragging && i == board.ui->dragIndex) continue; 

        sf::Sprite sprite(textureMap[board.getRenderBoardState()[i]]);
        sprite.setPosition(squares[i].getPosition());
        sprite.setScale(2.0f, 2.0f);
        target.draw(sprite);
    }

    if (board.ui->isDragging && board.ui->dragIndex != -1) {
        sf::Sprite dragSprite(textureMap[board.getRenderBoardState()[board.ui->dragIndex]]);
        dragSprite.setScale(2.0f, 2.0f);
        dragSprite.setOrigin(25.f, 25.f); 
        dragSprite.setPosition(board.ui->currentMousePos);
        target.draw(dragSprite);
    }
}

void ChessboardRenderer::drawPromotionDialog(sf::RenderWindow& target) {
    sf::RectangleShape overlay(sf::Vector2f(1200, 900));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    target.draw(overlay);
    target.draw(promoBox);
    for (auto& s : promoSprites) target.draw(s);
}

void ChessboardRenderer::applyScreenShake(sf::RenderWindow& target) {
    sf::View view = target.getDefaultView();
    if (board.shakeTimer > 0) {
        float rx = (static_cast<float>(std::rand() % 10) - 5.0f) * board.shakeIntensity;
        float ry = (static_cast<float>(std::rand() % 10) - 5.0f) * board.shakeIntensity;
        view.move(rx, ry);
    }
    target.setView(view);
}

void ChessboardRenderer::playMoveSound() {
    moveSound.play();
}

void ChessboardRenderer::playCaptureSound() {
    captureSound.play();
}

void ChessboardRenderer::playCheckSound() {
    checkSound.play();
}

void ChessboardRenderer::applyTheme() {
    // Theme implementation can be added here
}
