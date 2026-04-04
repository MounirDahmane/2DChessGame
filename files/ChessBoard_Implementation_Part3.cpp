// CONTINUATION OF ChessBoard_Enhanced.cpp  
// PART 3: AI, UTILITIES, AND ASSET LOADING

// ============================================================================
// AI FUNCTIONALITY
// ============================================================================

void ChessBoard::aiMove() {
    // Only if it's AI's turn (black) and no promotion pending
    if (!isAiGame || isWhiteTurn || isPromoting || aiMoveTimer > 0.0f) {
        return;
    }

    // Find best move using AI engine
    auto [from, to] = aiEngine->findBestMove(*this);

    if (from != -1 && to != -1) {
        selectedIndex = from;
        executeMoveWithEffects(from, to);
        selectedIndex = Chess::NO_VALID_INDEX;
        possibleMoves.clear();
        aiMoveTimer = Chess::AI_MOVE_DELAY;
        
        std::cout << "AI move: " << from << " -> " << to 
                  << " (Nodes: " << aiEngine->getNodesEvaluated() << ")\n";
    }
}

// ============================================================================
// UTILITY METHODS
// ============================================================================

int ChessBoard::getPieceValue(int8_t piece) const {
    int p = std::abs(piece);
    switch (p) {
        case Chess::WHITE_PAWN: return Chess::PAWN_VALUE;
        case Chess::WHITE_BISHOP: return Chess::BISHOP_VALUE;
        case Chess::WHITE_KNIGHT: return Chess::KNIGHT_VALUE;
        case Chess::WHITE_ROOK: return Chess::ROOK_VALUE;
        case Chess::WHITE_QUEEN: return Chess::QUEEN_VALUE;
        case Chess::WHITE_KING: return Chess::KING_VALUE;
        default: return 0;
    }
}

void ChessBoard::addMoveToLog(int from, int to, int8_t piece) {
    auto indexToCoord = [](int idx) {
        char file = 'a' + (idx % 8);
        char rank = '8' - (idx / 8);
        return std::string() + file + rank;
    };

    std::string move = indexToCoord(from) + "-" + indexToCoord(to);
    moveHistory.push_back(Move{from, to, 0, false, false, 0, move});
    moveNotationHistory.push_back(move);

    if (moveNotationHistory.size() > Chess::MAX_MOVE_HISTORY) {
        moveNotationHistory.erase(moveNotationHistory.begin());
    }
    
    gameStats.moveHistory.push_back(move);
}

void ChessBoard::triggerShake(float intensity, float duration) {
    shakeIntensity = intensity;
    shakeTimer = duration;
}

std::string ChessBoard::getBoardPositionHash() const {
    std::string hash;
    for (int i = 0; i < Chess::BOARD_SQUARES; ++i) {
        hash += std::to_string(boardState[i]) + ",";
    }
    return hash;
}

void ChessBoard::updateGameStats() {
    gameStats.totalMoves = moveHistory.size();
    gameStats.gameDuration = static_cast<float>(std::time(nullptr) - gameStats.startTime);
    gameStats.movesSinceLastCapture = movesSinceLastCaptureOrPawnMove;
}

void ChessBoard::finalizeGameStats() {
    updateGameStats();
    gameStats.finalizeGame();
}

// ============================================================================
// ASSET LOADING
// ============================================================================

void ChessBoard::loadAssets() {
    std::unordered_map<int8_t, std::string> fileMap = {
        {Chess::BLACK_PAWN, "blackPawn"},
        {Chess::BLACK_BISHOP, "blackBishop"},
        {Chess::BLACK_KNIGHT, "blackKnight"},
        {Chess::BLACK_ROOK, "blackRook"},
        {Chess::BLACK_KING, "blackKing"},
        {Chess::BLACK_QUEEN, "blackQueen"},
        {Chess::WHITE_PAWN, "whitePawn"},
        {Chess::WHITE_BISHOP, "whiteBishop"},
        {Chess::WHITE_KNIGHT, "whiteKnight"},
        {Chess::WHITE_ROOK, "whiteRook"},
        {Chess::WHITE_KING, "whiteKing"},
        {Chess::WHITE_QUEEN, "whiteQueen"}
    };

    for (const auto& [code, name] : fileMap) {
        std::string path = "assets/images/" + name + ".png";
        if (!textureMap[code].loadFromFile(path)) {
            std::cerr << "ERROR: Failed to load " << path << std::endl;
        }
    }

    if (!backgroundTexture.loadFromFile("assets/images/landscape.jpg")) {
        std::cerr << "ERROR: Failed to load background\n";
    }

    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "ERROR: Failed to load font\n";
    }

    if (!moveSoundBuffer.loadFromFile("assets/sounds/move.wav")) {
        std::cerr << "ERROR: Failed to load move sound\n";
    } else {
        moveSound.setBuffer(moveSoundBuffer);
    }

    if (!captureSoundBuffer.loadFromFile("assets/sounds/capture.wav")) {
        std::cerr << "ERROR: Failed to load capture sound\n";
    } else {
        captureSound.setBuffer(captureSoundBuffer);
    }

    if (!checkSoundBuffer.loadFromFile("assets/sounds/check.wav")) {
        std::cerr << "WARNING: Check sound not found\n";
    } else {
        checkSound.setBuffer(checkSoundBuffer);
    }
}

void ChessBoard::initLabels() {
    const std::string FILES = "abcdefgh";
    const std::string RANKS = "87654321";

    for (int i = 0; i < 8; ++i) {
        sf::Text fileLabel(FILES.substr(i, 1), font, 24);
        fileLabel.setPosition(
            i * Chess::PIXEL_WIDTH + Chess::OFFSET_X + 35,
            8 * Chess::PIXEL_WIDTH + Chess::OFFSET_Y + 10
        );
        fileLabel.setFillColor(Chess::LABEL_COLOR);
        labels.push_back(fileLabel);

        sf::Text rankLabel(RANKS.substr(i, 1), font, 24);
        rankLabel.setPosition(
            Chess::OFFSET_X - 35, 
            i * Chess::PIXEL_WIDTH + Chess::OFFSET_Y + 30
        );
        rankLabel.setFillColor(Chess::LABEL_COLOR);
        labels.push_back(rankLabel);
    }
}

void ChessBoard::syncPiecesWithBoard() {
    pieceSprites.clear();
    for (int i = 0; i < Chess::BOARD_SQUARES; ++i) {
        if (boardState[i] != Chess::EMPTY_SQUARE) {
            sf::Sprite sprite(textureMap[boardState[i]]);
            sprite.setPosition(squares[i].getPosition());
            sprite.setScale(2.0f, 2.0f);
            pieceSprites.push_back(sprite);
        }
    }
}

void ChessBoard::setupMenuUI() {
    menuButtons.clear();
    menuButtonTexts.clear();

    // PvP Button
    sf::RectangleShape pvpBtn({360, 80});
    pvpBtn.setFillColor(Chess::BUTTON_COLOR);
    pvpBtn.setOutlineThickness(2);
    pvpBtn.setOutlineColor(sf::Color::White);
    pvpBtn.setPosition(420, 300);
    menuButtons.push_back(pvpBtn);

    sf::Text pvpText("PLAYER VS PLAYER", font, 24);
    pvpText.setFillColor(sf::Color::White);
    centerText(pvpText, 600, 320);
    menuButtonTexts.push_back(pvpText);

    // PvAI Button
    sf::RectangleShape aiBtn({360, 80});
    aiBtn.setFillColor(Chess::BUTTON_COLOR);
    aiBtn.setOutlineThickness(2);
    aiBtn.setOutlineColor(sf::Color::White);
    aiBtn.setPosition(420, 420);
    menuButtons.push_back(aiBtn);

    sf::Text aiText("PLAYER VS AI", font, 24);
    aiText.setFillColor(sf::Color::White);
    centerText(aiText, 600, 440);
    menuButtonTexts.push_back(aiText);

    // Title
    menuTitle.setFont(font);
    menuTitle.setString("CHESS 2D");
    menuTitle.setCharacterSize(100);
    menuTitle.setFillColor(Chess::WINNER_GOLD);
    centerText(menuTitle, 600, 80);
}

void ChessBoard::setupDifficultyUI() {
    difficultyButtons.clear();
    difficultyTexts.clear();

    const char* difficulties[] = {"Easy", "Medium", "Hard", "Impossible"};
    const int numDifficulties = 4;
    const float buttonWidth = 200;
    const float buttonHeight = 80;
    const float spacing = 40;
    const float totalWidth = numDifficulties * buttonWidth + (numDifficulties - 1) * spacing;
    const float startX = (Chess::WINDOW_WIDTH - totalWidth) / 2;

    for (int i = 0; i < numDifficulties; ++i) {
        sf::RectangleShape btn({buttonWidth, buttonHeight});
        btn.setFillColor(Chess::BUTTON_COLOR);
        btn.setOutlineThickness(2);
        btn.setOutlineColor(sf::Color::White);
        btn.setPosition(startX + i * (buttonWidth + spacing), 400);
        difficultyButtons.push_back(btn);

        sf::Text text(difficulties[i], font, 20);
        text.setFillColor(sf::Color::White);
        centerText(text, startX + i * (buttonWidth + spacing) + buttonWidth / 2, 420);
        difficultyTexts.push_back(text);
    }
}

void ChessBoard::setupPromotionUI() {
    promotionButtons.clear();
    promotionTexts.clear();

    const char* pieces[] = {"Queen", "Rook", "Bishop", "Knight"};
    const int numPieces = 4;
    const float buttonWidth = 120;
    const float buttonHeight = 120;
    const float spacing = 40;
    const float totalWidth = numPieces * buttonWidth + (numPieces - 1) * spacing;
    const float startX = (Chess::WINDOW_WIDTH - totalWidth) / 2;

    for (int i = 0; i < numPieces; ++i) {
        sf::RectangleShape btn({buttonWidth, buttonHeight});
        btn.setFillColor(Chess::BUTTON_COLOR);
        btn.setOutlineThickness(2);
        btn.setOutlineColor(Chess::TURN_COLOR);
        btn.setPosition(startX + i * (buttonWidth + spacing), 350);
        promotionButtons.push_back(btn);

        sf::Text text(pieces[i], font, 18);
        text.setFillColor(sf::Color::White);
        centerText(text, startX + i * (buttonWidth + spacing) + buttonWidth / 2, 395);
        promotionTexts.push_back(text);
    }
}

std::optional<int> ChessBoard::getIndexFromMouse(const sf::Vector2i& mousePos) {
    int col = (mousePos.x - static_cast<int>(Chess::OFFSET_X)) / 
              static_cast<int>(Chess::PIXEL_WIDTH);
    int row = (mousePos.y - static_cast<int>(Chess::OFFSET_Y)) / 
              static_cast<int>(Chess::PIXEL_WIDTH);

    if (col >= 0 && col < Chess::BOARD_SIZE && row >= 0 && row < Chess::BOARD_SIZE) {
        return row * Chess::BOARD_SIZE + col;
    }
    return std::nullopt;
}

void ChessBoard::centerText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.width / 2.0f, 0);
    text.setPosition(x, y);
}
