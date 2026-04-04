#pragma once

#include "ChessBoard.hpp"
#include <SFML/Window.hpp>
#include <optional>
#include <vector>

class ChessBoard;

/**
 * @brief Handles all input and user interaction with the chess board.
 * Separates UI interaction logic from core game logic.
 */
class ChessboardUI {
public:
    explicit ChessboardUI(ChessBoard& board);
    
    // =========================================================================
    // Input & Interaction
    // =========================================================================
    void handleHover(const sf::Vector2i& mousePos);
    void handleClick(const sf::Vector2i& mousePos);
    void handleRelease(const sf::Vector2i& mousePos);
    
    // =========================================================================
    // UI State & Helpers
    // =========================================================================
    void calculatePossibleMoves(int startIdx);
    std::optional<int> getIndexFromMouse(const sf::Vector2i& mPos);
    
    // Accessors
    int getSelectedIndex() const { return selectedIndex; }
    int getHoveredIndex() const { return hoveredIndex; }
    const std::vector<MoveHint>& getPossibleMoves() const { return possibleMoves; }
    
    // Setters
    void setSelectedIndex(int idx) { selectedIndex = idx; }
    void setHoveredIndex(int idx) { hoveredIndex = idx; }
    void clearPossibleMoves() { possibleMoves.clear(); }

    // UI State members (public for renderer access)
    bool isDragging = false; 
    int dragIndex = -1;      
    sf::Vector2f currentMousePos;

private:
    ChessBoard& board;
    
    // =========================================================================
    // UI State
    // =========================================================================
    int hoveredIndex = -1;
    int selectedIndex = -1;
    
    std::vector<MoveHint> possibleMoves;
    
    // =========================================================================
    // Menu Input Handling
    // =========================================================================
    void handleMenuClick(const sf::Vector2i& mousePos);
    void handleInGameUIClick(const sf::Vector2i& mousePos);
    
    friend class ChessBoard;
    friend class ChessboardRenderer;
};
