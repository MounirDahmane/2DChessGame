#include "ChessboardUI.hpp"
#include "ChessBoard.hpp"

ChessboardUI::ChessboardUI(ChessBoard& board) : board(board) {}

void ChessboardUI::handleHover(const sf::Vector2i& mPos) {
    if (board.isAiThinking) return; 
    currentMousePos = sf::Vector2f(mPos);
    auto idx = getIndexFromMouse(mPos);
    hoveredIndex = idx ? *idx : -1;
}

void ChessboardUI::handleClick(const sf::Vector2i& mPos) {
    if (board.getState() == ChessBoard::GameState::MENU) {
        handleMenuClick(mPos);
        return;
    }

    if (board.isAnimating || board.isAiThinking) return; 

    if (board.getIsPromoting()) {
        handleInGameUIClick(mPos);
        return;
    }

    handleInGameUIClick(mPos);
}

void ChessboardUI::handleMenuClick(const sf::Vector2i& mPos) {
    // Menu button handling is managed by ChessboardRenderer
    // This would contain the logic for menu clicks if separated
}

void ChessboardUI::handleInGameUIClick(const sf::Vector2i& mPos) {
    // Check menu button through renderer (needs access to bounds)
    // For now, this is simplified - the actual bounds checking happens in renderer level
    
    if (board.getIsPromoting()) {
        // Promotion click handling - done in renderer
        return;
    }

    if (board.getIsCheckmate() || board.isStalemate) return;

    auto idx = getIndexFromMouse(mPos);
    if (!idx) return;
    int target_idx = *idx;

    if (selectedIndex == -1) {
        if (board.boardState[target_idx] != 0 && board.isWhite(board.boardState[target_idx]) == board.getIsWhiteTurn()) {
            selectedIndex = target_idx; 
            isDragging = true;
            dragIndex = target_idx;
            calculatePossibleMoves(selectedIndex);
        }
    } else {
        if (board.validateMove(selectedIndex, target_idx)) {
            board.executeMoveWithEffects(selectedIndex, target_idx);
            selectedIndex = -1; 
            possibleMoves.clear();
        } else if (board.boardState[target_idx] != 0 && board.isSameTeam(board.boardState[selectedIndex], board.boardState[target_idx])) {
            selectedIndex = target_idx; 
            isDragging = true;
            dragIndex = target_idx;
            calculatePossibleMoves(selectedIndex);
        } else {
            selectedIndex = -1; 
            possibleMoves.clear();
        }
    }
}

void ChessboardUI::handleRelease(const sf::Vector2i& mPos) {
    if (!isDragging || board.isAiThinking) return;
    isDragging = false;

    auto idx = getIndexFromMouse(mPos);
    if (idx && *idx != dragIndex) {
        if (board.validateMove(dragIndex, *idx)) {
            board.executeMoveWithEffects(dragIndex, *idx);
            selectedIndex = -1;
            possibleMoves.clear();
        }
    }
    dragIndex = -1;
}

void ChessboardUI::calculatePossibleMoves(int from) {
    possibleMoves.clear();
    for (int to = 0; to < Chess::BOARD_SQUARES; ++to) {
        if (board.validateMove(from, to, false)) {
            possibleMoves.push_back({to, (board.boardState[to] != Chess::EMPTY_SQUARE || to == board.enPassantIndex)});
        }
    }
}

std::optional<int> ChessboardUI::getIndexFromMouse(const sf::Vector2i& mousePos) {
    int col = (mousePos.x - static_cast<int>(Chess::OFFSET_X)) / static_cast<int>(Chess::PIXEL_WIDTH);
    int row = (mousePos.y - static_cast<int>(Chess::OFFSET_Y)) / static_cast<int>(Chess::PIXEL_WIDTH);

    if (col >= 0 && col < Chess::BOARD_SIZE && row >= 0 && row < Chess::BOARD_SIZE) {
        return row * Chess::BOARD_SIZE + col;
    }
    return std::nullopt;
}
