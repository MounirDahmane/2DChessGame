#include "ChessBoard_Enhanced.hpp"
#include <algorithm>
#include <climits>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

// ============================================================================
// CONSTRUCTOR & INITIALIZATION
// ============================================================================

ChessBoard::ChessBoard() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Initialize AI engine
    aiEngine = std::make_unique<ChessAIEngine>(aiDifficulty);
    
    // Initialize move hint circle
    moveHintCircle.setRadius(15.0f);
    moveHintCircle.setOrigin(15.0f, 15.0f);

    // Load all assets
    loadAssets();
    initLabels();

    // Create board squares
    for (int i = 0; i < Chess::BOARD_SQUARES; ++i) {
        auto& square = squares.emplace_back(
            sf::Vector2f(Chess::PIXEL_WIDTH, Chess::PIXEL_WIDTH)
        );
        int row = i / Chess::BOARD_SIZE;
        int col = i % Chess::BOARD_SIZE;
        
        square.setPosition(
            col * Chess::PIXEL_WIDTH + Chess::OFFSET_X,
            row * Chess::PIXEL_WIDTH + Chess::OFFSET_Y
        );
        
        bool isWhiteSquare = ((row + col) % 2 == 0);
        square.setFillColor(isWhiteSquare ? Chess::WHITE_SQUARE : Chess::BLACK_SQUARE);
    }

    // Setup UI
    setupMenuUI();
    setupDifficultyUI();
    setupPromotionUI();
    
    // Initialize game stats
    gameStats = GameStatistics();
    
    resetGame();
}

ChessBoard::~ChessBoard() {
    // Cleanup handled by unique_ptr
}

void ChessBoard::setLocalPlayerName(const std::string& name) {
    gameStats.whitePlayer = name;
}

void ChessBoard::setOpponentName(const std::string& name) {
    gameStats.blackPlayer = name;
}

void ChessBoard::setAIDifficulty(Chess::Difficulty diff) {
    aiDifficulty = diff;
    if (aiEngine) {
        aiEngine->setDifficulty(static_cast<ChessAIEngine::Difficulty>(diff));
    }
}

const char* ChessBoard::getAIDifficultyName() const {
    return ChessAIEngine::getDifficultyName(static_cast<ChessAIEngine::Difficulty>(aiDifficulty));
}

void ChessBoard::resetGame() {
    // Initialize board with standard chess starting position
    boardState = {{
        Chess::BLACK_ROOK,   Chess::BLACK_KNIGHT, Chess::BLACK_BISHOP, Chess::BLACK_QUEEN,
        Chess::BLACK_KING,   Chess::BLACK_BISHOP, Chess::BLACK_KNIGHT, Chess::BLACK_ROOK,
        Chess::BLACK_PAWN,   Chess::BLACK_PAWN,   Chess::BLACK_PAWN,   Chess::BLACK_PAWN,
        Chess::BLACK_PAWN,   Chess::BLACK_PAWN,   Chess::BLACK_PAWN,   Chess::BLACK_PAWN,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        Chess::WHITE_PAWN,   Chess::WHITE_PAWN,   Chess::WHITE_PAWN,   Chess::WHITE_PAWN,
        Chess::WHITE_PAWN,   Chess::WHITE_PAWN,   Chess::WHITE_PAWN,   Chess::WHITE_PAWN,
        Chess::WHITE_ROOK,   Chess::WHITE_KNIGHT, Chess::WHITE_BISHOP, Chess::WHITE_QUEEN,
        Chess::WHITE_KING,   Chess::WHITE_BISHOP, Chess::WHITE_KNIGHT, Chess::WHITE_ROOK
    }};

    // Reset game state
    isWhiteTurn = true;
    isCheckmate = false;
    isStaleMate = false;
    selectedIndex = Chess::NO_VALID_INDEX;
    hoveredIndex = Chess::NO_VALID_INDEX;
    enPassantIndex = Chess::NO_VALID_INDEX;
    isPromoting = false;
    promotionIndex = Chess::NO_VALID_INDEX;
    isPaused = false;
    inCheck = false;

    // Reset time
    whiteTime = Chess::DEFAULT_TIME_LIMIT;
    blackTime = Chess::DEFAULT_TIME_LIMIT;
    whiteScore = 0;
    blackScore = 0;

    // Reset castling rights
    whiteKingMoved = false;
    whiteRookRightMoved = false;
    whiteRookLeftMoved = false;
    blackKingMoved = false;
    blackRookRightMoved = false;
    blackRookLeftMoved = false;

    // Reset special rules counters
    movesSinceLastCaptureOrPawnMove = 0;

    // Clear history
    capturedByWhite.clear();
    capturedByBlack.clear();
    moveHistory.clear();
    undoneHistory.clear();
    moveNotationHistory.clear();
    boardHistory.clear();
    boardPositionCount.clear();
    possibleMoves.clear();

    // Update visuals
    syncPiecesWithBoard();
    
    // Initialize game stats
    gameStats.whitePlayer = "Player 1";
    gameStats.blackPlayer = isAiGame ? "AI" : "Player 2";
    gameStats.result = GameStatistics::ONGOING;
    gameStats.startTime = std::time(nullptr);
}

void ChessBoard::pauseGame() {
    isPaused = true;
    std::cout << "Game paused\n";
}

void ChessBoard::resumeGame() {
    isPaused = false;
    std::cout << "Game resumed\n";
}

// ============================================================================
// UPDATE & RENDERING
// ============================================================================

void ChessBoard::update(float dt) {
    // Update shake effect
    if (shakeTimer > 0) {
        shakeTimer -= dt;
    } else {
        shakeTimer = 0.0f;
        shakeIntensity = 0.0f;
    }

    // Skip updates during menu or promotion or pause
    if (currentState == Chess::GameState::MENU || 
        currentState == Chess::GameState::DIFFICULTY_SELECT ||
        isCheckmate || isStaleMate || isPromoting || isPaused) {
        return;
    }

    // Update player timers
    if (isWhiteTurn) {
        whiteTime -= dt;
    } else {
        blackTime -= dt;
    }

    // Check for timeout
    if (whiteTime <= 0.0f || blackTime <= 0.0f) {
        whiteTime = std::max(0.0f, whiteTime);
        blackTime = std::max(0.0f, blackTime);
        isCheckmate = true;
        triggerShake(5.0f, 0.4f);
        
        if (whiteTime <= 0.0f) {
            gameStats.result = GameStatistics::BLACK_WIN;
        } else {
            gameStats.result = GameStatistics::WHITE_WIN;
        }
    }

    // Update AI move delay
    if (aiMoveTimer > 0.0f) {
        aiMoveTimer -= dt;
    }
}

void ChessBoard::draw(sf::RenderWindow& target) {
    target.setView(target.getDefaultView());

    switch (currentState) {
        case Chess::GameState::MENU:
            drawMenu(target);
            break;
        case Chess::GameState::DIFFICULTY_SELECT:
            drawDifficultySelection(target);
            break;
        case Chess::GameState::PLAYING:
        case Chess::GameState::PAUSED:
        case Chess::GameState::GAME_OVER:
            drawBoard(target);
            drawSidebar(target);
            drawPieces(target);
            if (isPromoting) {
                drawPromotionDialog(target);
            }
            if (isPaused) {
                sf::RectangleShape pauseOverlay(
                    sf::Vector2f(Chess::WINDOW_WIDTH, Chess::WINDOW_HEIGHT)
                );
                pauseOverlay.setFillColor(sf::Color(0, 0, 0, 100));
                target.draw(pauseOverlay);
                
                sf::Text pauseText("GAME PAUSED", font, 80);
                pauseText.setFillColor(Chess::WINNER_GOLD);
                centerText(pauseText, 600, 400);
                target.draw(pauseText);
            }
            break;
        default:
            break;
    }

    target.setView(target.getDefaultView());
}

void ChessBoard::drawMenu(sf::RenderWindow& target) {
    target.draw(sf::Sprite(backgroundTexture));
    target.draw(menuTitle);
    
    for (size_t i = 0; i < menuButtons.size(); ++i) {
        target.draw(menuButtons[i]);
        target.draw(menuButtonTexts[i]);
    }
}

void ChessBoard::drawDifficultySelection(sf::RenderWindow& target) {
    target.draw(sf::Sprite(backgroundTexture));
    
    sf::Text title("SELECT DIFFICULTY", font, 80);
    title.setFillColor(Chess::WINNER_GOLD);
    centerText(title, 600, 100);
    target.draw(title);
    
    for (size_t i = 0; i < difficultyButtons.size(); ++i) {
        target.draw(difficultyButtons[i]);
        target.draw(difficultyTexts[i]);
    }
}

void ChessBoard::drawBoard(sf::RenderWindow& target) {
    // Apply screen shake effect
    applyScreenShake(target);

    target.draw(sf::Sprite(backgroundTexture));

    // Draw squares
    for (auto& square : squares) {
        target.draw(square);
    }

    // Draw board labels
    for (auto& label : labels) {
        target.draw(label);
    }

    // Draw selection highlight
    if (selectedIndex != Chess::NO_VALID_INDEX) {
        sf::RectangleShape selectionBox(
            sf::Vector2f(Chess::PIXEL_WIDTH, Chess::PIXEL_WIDTH)
        );
        selectionBox.setPosition(squares[selectedIndex].getPosition());
        selectionBox.setFillColor(Chess::SELECT_COLOR);
        target.draw(selectionBox);
    }

    // Draw move hints
    for (const auto& hint : possibleMoves) {
        moveHintCircle.setPosition(
            squares[hint.index].getPosition() + 
            sf::Vector2f(Chess::PIXEL_WIDTH / 2, Chess::PIXEL_WIDTH / 2)
        );
        moveHintCircle.setFillColor(hint.isCapture ? Chess::HINT_CAPTURE : Chess::HINT_QUIET);
        target.draw(moveHintCircle);
    }
}

void ChessBoard::drawSidebar(sf::RenderWindow& target) {
    // Black side panel (top)
    sf::RectangleShape topPanel(sf::Vector2f(Chess::SIDEBAR_WIDTH, 450));
    topPanel.setPosition(Chess::SIDEBAR_X, 0);
    topPanel.setFillColor(Chess::SIDEBAR_PANEL_BLACK);
    target.draw(topPanel);

    // White side panel (bottom)
    sf::RectangleShape bottomPanel(sf::Vector2f(Chess::SIDEBAR_WIDTH, 450));
    bottomPanel.setPosition(Chess::SIDEBAR_X, 450);
    bottomPanel.setFillColor(Chess::SIDEBAR_PANEL_WHITE);
    target.draw(bottomPanel);

    // Turn indicator bar
    sf::RectangleShape turnIndicator(sf::Vector2f(8, 450));
    turnIndicator.setFillColor(Chess::TURN_COLOR);
    turnIndicator.setPosition(Chess::SIDEBAR_X, isWhiteTurn ? 450 : 0);
    if (!isCheckmate && !isStaleMate) {
        target.draw(turnIndicator);
    }

    // Helper lambda for time formatting
    auto formatTime = [](float t) {
        int minutes = static_cast<int>(t) / 60;
        int seconds = static_cast<int>(t) % 60;
        std::stringstream ss;
        ss << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;
        return ss.str();
    };

    sf::Text sidebarText;
    sidebarText.setFont(font);

    // Black timer (top)
    bool blackWins = (isCheckmate && (isWhiteTurn || whiteTime <= 0)) ||
                     (isStaleMate && !isWhiteTurn);
    sidebarText.setCharacterSize(55);
    sidebarText.setFillColor(blackWins ? Chess::WINNER_GOLD : sf::Color::White);
    sidebarText.setString(formatTime(blackTime));
    centerText(sidebarText, Chess::SIDEBAR_X + 150, 40);
    target.draw(sidebarText);

    if (blackWins) {
        sidebarText.setCharacterSize(22);
        sidebarText.setString("WINNER");
        centerText(sidebarText, Chess::SIDEBAR_X + 150, 15);
        target.draw(sidebarText);
    }

    // Display game state
    sidebarText.setCharacterSize(16);
    sidebarText.setFillColor(sf::Color::Yellow);
    std::string gameState;
    if (isCheckmate) gameState = isWhiteTurn ? "BLACK WINS" : "WHITE WINS";
    else if (isStaleMate) gameState = "STALEMATE";
    else if (inCheck) gameState = isWhiteTurn ? "WHITE IN CHECK" : "BLACK IN CHECK";
    else gameState = "";
    
    if (!gameState.empty()) {
        sidebarText.setString(gameState);
        centerText(sidebarText, Chess::SIDEBAR_X + 150, 130);
        target.draw(sidebarText);
    }

    // Black score and captured pieces
    sidebarText.setCharacterSize(18);
    sidebarText.setFillColor(sf::Color(180, 180, 180));
    sidebarText.setOrigin(0, 0);
    sidebarText.setString("Score: +" + std::to_string(blackScore));
    sidebarText.setPosition(Chess::SIDEBAR_X + 25, 170);
    target.draw(sidebarText);

    for (size_t i = 0; i < capturedByBlack.size(); ++i) {
        sf::Sprite capturedSprite(textureMap[capturedByBlack[i]]);
        capturedSprite.setScale(0.6f, 0.6f);
        capturedSprite.setPosition(
            Chess::SIDEBAR_X + 25 + (i % 5) * 52,
            200 + (i / 5) * 55
        );
        target.draw(capturedSprite);
    }

    // White timer (bottom)
    bool whiteWins = (isCheckmate && (!isWhiteTurn || blackTime <= 0)) ||
                     (isStaleMate && isWhiteTurn);
    sidebarText.setCharacterSize(55);
    sidebarText.setFillColor(whiteWins ? Chess::WINNER_GOLD : sf::Color::White);
    sidebarText.setString(formatTime(whiteTime));
    centerText(sidebarText, Chess::SIDEBAR_X + 150, 785);
    target.draw(sidebarText);

    if (whiteWins) {
        sidebarText.setCharacterSize(22);
        sidebarText.setString("WINNER");
        centerText(sidebarText, Chess::SIDEBAR_X + 150, 855);
        target.draw(sidebarText);
    }

    // White score and captured pieces
    sidebarText.setCharacterSize(18);
    sidebarText.setFillColor(sf::Color(180, 180, 180));
    sidebarText.setOrigin(0, 0);
    sidebarText.setString("Score: +" + std::to_string(whiteScore));
    sidebarText.setPosition(Chess::SIDEBAR_X + 25, 755);
    target.draw(sidebarText);

    for (size_t i = 0; i < capturedByWhite.size(); ++i) {
        sf::Sprite capturedSprite(textureMap[capturedByWhite[i]]);
        capturedSprite.setScale(0.6f, 0.6f);
        capturedSprite.setPosition(
            Chess::SIDEBAR_X + 25 + (i % 5) * 52,
            600 + (i / 5) * 55
        );
        target.draw(capturedSprite);
    }

    // Move history log
    sidebarText.setCharacterSize(12);
    sidebarText.setFillColor(sf::Color(150, 150, 150));
    sidebarText.setOrigin(0, 0);
    sidebarText.setString("Moves: " + std::to_string(moveHistory.size()));
    sidebarText.setPosition(Chess::SIDEBAR_X + 20, 410);
    target.draw(sidebarText);
    
    sidebarText.setString("50-Move: " + std::to_string(movesSinceLastCaptureOrPawnMove));
    sidebarText.setPosition(Chess::SIDEBAR_X + 20, 430);
    target.draw(sidebarText);
}

void ChessBoard::drawPieces(sf::RenderWindow& target) {
    for (auto& piece : pieceSprites) {
        target.draw(piece);
    }
}

void ChessBoard::drawPromotionDialog(sf::RenderWindow& target) {
    // Semi-transparent overlay
    sf::RectangleShape overlay(
        sf::Vector2f(Chess::WINDOW_WIDTH, Chess::WINDOW_HEIGHT)
    );
    overlay.setFillColor(Chess::MODAL_OVERLAY);
    target.draw(overlay);

    // Draw promotion piece selection
    sf::Text title("SELECT PROMOTION PIECE", font, 32);
    title.setFillColor(Chess::WINNER_GOLD);
    centerText(title, 600, 200);
    target.draw(title);

    for (size_t i = 0; i < promotionButtons.size(); ++i) {
        target.draw(promotionButtons[i]);
        target.draw(promotionTexts[i]);
    }
}

void ChessBoard::applyScreenShake(sf::RenderWindow& target) {
    if (shakeTimer > 0) {
        sf::View view = target.getDefaultView();
        float offsetX = (std::rand() % 10 - 5.0f) * shakeIntensity;
        float offsetY = (std::rand() % 10 - 5.0f) * shakeIntensity;
        view.move(offsetX, offsetY);
        target.setView(view);
    }
}

// ============================================================================
// EVENT HANDLING
// ============================================================================

void ChessBoard::handleClick(const sf::Vector2i& mousePos) {
    // Handle menu clicks
    if (currentState == Chess::GameState::MENU) {
        for (size_t i = 0; i < menuButtons.size(); ++i) {
            if (menuButtons[i].getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                if (i == 0) { // PvP
                    setAiMode(false);
                    setGameState(Chess::GameState::PLAYING);
                    resetGame();
                } else if (i == 1) { // PvAI
                    setAiMode(true);
                    setGameState(Chess::GameState::DIFFICULTY_SELECT);
                }
            }
        }
        return;
    }

    // Handle difficulty selection
    if (currentState == Chess::GameState::DIFFICULTY_SELECT) {
        for (size_t i = 0; i < difficultyButtons.size(); ++i) {
            if (difficultyButtons[i].getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                setAIDifficulty(static_cast<Chess::Difficulty>(Chess::Difficulty::EASY + i * 2));
                setGameState(Chess::GameState::PLAYING);
                resetGame();
            }
        }
        return;
    }

    // Handle game board clicks
    if (currentState != Chess::GameState::PLAYING) {
        return;
    }

    // Handle promotion piece selection
    if (isPromoting) {
        for (size_t i = 0; i < promotionButtons.size(); ++i) {
            if (promotionButtons[i].getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                int8_t pieces[] = {Chess::WHITE_QUEEN, Chess::WHITE_ROOK, 
                                   Chess::WHITE_BISHOP, Chess::WHITE_KNIGHT};
                int8_t promotionPiece = isWhite(boardState[promotionIndex]) ? 
                                       pieces[i] : -pieces[i];
                promoteSelectedPawn(promotionPiece);
            }
        }
        return;
    }

    // Get board index from mouse position
    auto idx = getIndexFromMouse(mousePos);
    if (!idx || isCheckmate || isStaleMate) {
        return;
    }

    int targetIdx = *idx;

    // First click - select a piece
    if (selectedIndex == Chess::NO_VALID_INDEX) {
        if (boardState[targetIdx] != Chess::EMPTY_SQUARE && 
            isWhite(boardState[targetIdx]) == isWhiteTurn) {
            selectedIndex = targetIdx;
            calculatePossibleMoves(selectedIndex);
        }
        return;
    }

    // Second click - move or reselect
    if (validateMove(selectedIndex, targetIdx)) {
        executeMoveWithEffects(selectedIndex, targetIdx);
        selectedIndex = Chess::NO_VALID_INDEX;
        possibleMoves.clear();
    } else if (boardState[targetIdx] != Chess::EMPTY_SQUARE && 
               isSameTeam(boardState[selectedIndex], boardState[targetIdx])) {
        selectedIndex = targetIdx;
        calculatePossibleMoves(selectedIndex);
    } else {
        selectedIndex = Chess::NO_VALID_INDEX;
        possibleMoves.clear();
    }
}

void ChessBoard::handleHover(const sf::Vector2i& mousePos) {
    auto idx = getIndexFromMouse(mousePos);
    hoveredIndex = idx ? *idx : Chess::NO_VALID_INDEX;
}

// ============================================================================
// UNDO/REDO FUNCTIONALITY
// ============================================================================

void ChessBoard::undoMove() {
    if (moveHistory.empty()) {
        std::cout << "No moves to undo\n";
        return;
    }

    Move lastMove = moveHistory.back();
    moveHistory.pop_back();
    undoneHistory.push_back(lastMove);

    // Restore board state
    boardState[lastMove.from] = boardState[lastMove.to];
    boardState[lastMove.to] = lastMove.capturedPiece;

    // Switch turn
    isWhiteTurn = !isWhiteTurn;

    // Reset game end conditions
    isCheckmate = false;
    isStaleMate = false;
    checkForGameEnd();

    syncPiecesWithBoard();
    std::cout << "Move undone: " << lastMove.notation << "\n";
}

void ChessBoard::redoMove() {
    if (undoneHistory.empty()) {
        std::cout << "No moves to redo\n";
        return;
    }

    Move moveToRedo = undoneHistory.back();
    undoneHistory.pop_back();
    
    // Re-execute the move
    executeMoveWithEffects(moveToRedo.from, moveToRedo.to);
    std::cout << "Move redone: " << moveToRedo.notation << "\n";
}

// ============================================================================
// PAWN PROMOTION
// ============================================================================

void ChessBoard::promoteSelectedPawn(int8_t promotionPiece) {
    if (promotionIndex != Chess::NO_VALID_INDEX) {
        boardState[promotionIndex] = promotionPiece;
        isPromoting = false;
        promotionIndex = Chess::NO_VALID_INDEX;
        gameStats.pawnPromotionCount++;

        // Switch turn and check game state
        isWhiteTurn = !isWhiteTurn;
        checkForGameEnd();

        syncPiecesWithBoard();
    }
}

// Continued in next part...
