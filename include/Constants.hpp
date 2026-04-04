#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>

/**
 * @file Constants.hpp
 * @brief Centralized configuration constants for the E2-E4 Chess Engine.
 * * Contains board dimensions, piece encodings, material values, visual colors,
 * and configuration settings to ensure consistency across the application.
 */
namespace Chess
{

// ============================================================================
// WINDOW & BOARD DIMENSIONS
// ============================================================================

/** @brief Total width of the application window in pixels. */
static constexpr float WINDOW_WIDTH = 1200.0f;

/** @brief Total height of the application window in pixels. */
static constexpr float WINDOW_HEIGHT = 900.0f;

/** @brief The width and height of a single chess square in pixels. */
static constexpr float PIXEL_WIDTH = 100.0f;

/** @brief The X-axis padding from the left edge of the window to the board. */
static constexpr float OFFSET_X = 50.0f;

/** @brief The Y-axis padding from the top edge of the window to the board. */
static constexpr float OFFSET_Y = 50.0f;

/** @brief The X-coordinate where the right-side UI sidebar begins. */
static constexpr float SIDEBAR_X = 900.0f;

/** @brief The total width of the right-side UI sidebar in pixels. */
static constexpr float SIDEBAR_WIDTH = 300.0f;

/** @brief Number of squares per row/column (Standard 8x8). */
static constexpr int BOARD_SIZE = 8;

/** @brief Total number of squares on the board. */
static constexpr int BOARD_SQUARES = 64;

// ============================================================================
// PIECE ENCODING (DATA REPRESENTATION)
// ============================================================================
// Positive numbers represent White pieces, Negative numbers represent Black.

static constexpr int8_t EMPTY_SQUARE = 0;

static constexpr int8_t WHITE_PAWN = 1;
static constexpr int8_t WHITE_BISHOP = 3;
static constexpr int8_t WHITE_KNIGHT = 4;
static constexpr int8_t WHITE_ROOK = 5;
static constexpr int8_t WHITE_KING = 7;
static constexpr int8_t WHITE_QUEEN = 9;

static constexpr int8_t BLACK_PAWN = -1;
static constexpr int8_t BLACK_BISHOP = -3;
static constexpr int8_t BLACK_KNIGHT = -4;
static constexpr int8_t BLACK_ROOK = -5;
static constexpr int8_t BLACK_KING = -7;
static constexpr int8_t BLACK_QUEEN = -9;

// ============================================================================
// PIECE MATERIAL VALUES (FOR AI EVALUATION)
// ============================================================================

/** @brief Standard centipawn value of a Pawn. */
static constexpr int PAWN_VALUE = 100;
/** @brief Standard centipawn value of a Bishop. */
static constexpr int BISHOP_VALUE = 320;
/** @brief Standard centipawn value of a Knight. */
static constexpr int KNIGHT_VALUE = 330;
/** @brief Standard centipawn value of a Rook. */
static constexpr int ROOK_VALUE = 500;
/** @brief Standard centipawn value of a Queen. */
static constexpr int QUEEN_VALUE = 900;
/** @brief Arbitrarily high value for the King to heavily penalize Checkmate states. */
static constexpr int KING_VALUE = 20000;

// ============================================================================
// SPECIAL SQUARES (INDEXED 0-63)
// ============================================================================

/** @brief Starting 1D array index of the White King (e1). */
static constexpr int WHITE_KING_START = 60;
/** @brief Starting 1D array index of the White Kingside Rook (h1). */
static constexpr int WHITE_ROOK_RIGHT_START = 63;
/** @brief Starting 1D array index of the White Queenside Rook (a1). */
static constexpr int WHITE_ROOK_LEFT_START = 56;

/** @brief Starting 1D array index of the Black King (e8). */
static constexpr int BLACK_KING_START = 4;
/** @brief Starting 1D array index of the Black Kingside Rook (h8). */
static constexpr int BLACK_ROOK_RIGHT_START = 7;
/** @brief Starting 1D array index of the Black Queenside Rook (a8). */
static constexpr int BLACK_ROOK_LEFT_START = 0;

/** @brief Sentinel value indicating no square is selected or targeted. */
static constexpr int NO_VALID_INDEX = -1;

// ============================================================================
// VISUAL COLORS & THEMES
// ============================================================================

// --- Board Colors (Classic Theme Defaults) ---
static const sf::Color BLACK_SQUARE(181, 136, 99);
static const sf::Color WHITE_SQUARE(240, 217, 181);

// --- User Interaction Colors ---
/** @brief Translucent highlight when hovering over a piece/square. */
static const sf::Color HOVER_COLOR(255, 255, 255, 50);
/** @brief Vibrant green overlay for the currently selected piece. */
static const sf::Color SELECT_COLOR(46, 204, 113, 130);
/** @brief Translucent black dot marking a valid non-capture move. */
static const sf::Color HINT_QUIET(0, 0, 0, 60);
/** @brief Translucent red ring/dot marking a valid capture move. */
static const sf::Color HINT_CAPTURE(255, 0, 0, 130);
/** @brief Color for the A-H and 1-8 board coordinate labels. */
static const sf::Color LABEL_COLOR(231, 76, 60);

// --- Sidebar & Modal UI Colors ---
static const sf::Color SIDEBAR_PANEL_BLACK(33, 37, 43);
static const sf::Color SIDEBAR_PANEL_WHITE(40, 44, 52);
/** @brief Neon indicator for the active player's turn line. */
static const sf::Color TURN_COLOR(0, 229, 255);
/** @brief Winning text and highlight color. */
static const sf::Color WINNER_GOLD(255, 215, 0);
/** @brief Screen darkening overlay for checkmate and promotion popups. */
static const sf::Color MODAL_OVERLAY(0, 0, 0, 180);

// ============================================================================
// GAMEPLAY & AI SETTINGS
// ============================================================================

/** @brief Default match time per player in seconds (10 minutes). */
static constexpr float DEFAULT_TIME_LIMIT = 600.0f;

/** @brief Maximum number of moves retained in the UI move log. */
static constexpr int MAX_MOVE_HISTORY = 50;

/** @brief Artificial delay (seconds) to prevent the AI from moving instantly. */
static constexpr float AI_MOVE_DELAY = 0.5f;

/** @brief Default search depth for the Minimax algorithm. */
static constexpr int AI_DEPTH = 4;

} // namespace Chess