#pragma once

#include <SFML/Graphics.hpp>

// ============================================================================
// CHESS ENGINE CONSTANTS - ENHANCED VERSION
// ============================================================================

namespace Chess {

// ========== WINDOW & BOARD SETUP ==========
static constexpr float PIXEL_WIDTH = 100.0f;
static constexpr float OFFSET_X = 50.0f;
static constexpr float OFFSET_Y = 50.0f;
static constexpr int BOARD_SIZE = 8;
static constexpr int BOARD_SQUARES = 64;
static constexpr float WINDOW_WIDTH = 1200.0f;
static constexpr float WINDOW_HEIGHT = 900.0f;

// ========== PIECE VALUES & ENCODING ==========
static constexpr int PAWN_VALUE = 100;
static constexpr int KNIGHT_VALUE = 330;
static constexpr int BISHOP_VALUE = 320;
static constexpr int ROOK_VALUE = 500;
static constexpr int QUEEN_VALUE = 900;
static constexpr int KING_VALUE = 20000;

// Piece encoding (positive = White, negative = Black)
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

static constexpr int8_t EMPTY_SQUARE = 0;

// ========== SPECIAL SQUARES ==========
static constexpr int WHITE_KING_START = 60;
static constexpr int WHITE_ROOK_RIGHT_START = 63;
static constexpr int WHITE_ROOK_LEFT_START = 56;
static constexpr int BLACK_KING_START = 4;
static constexpr int BLACK_ROOK_RIGHT_START = 7;
static constexpr int BLACK_ROOK_LEFT_START = 0;

// ========== UI LAYOUT ==========
static constexpr float SIDEBAR_X = 900.0f;
static constexpr float SIDEBAR_WIDTH = 300.0f;

// ========== COLORS - BOARD ==========
static const sf::Color BLACK_SQUARE(181, 136, 99);
static const sf::Color WHITE_SQUARE(240, 217, 181);

// ========== COLORS - UI ==========
static const sf::Color HOVER_COLOR(255, 255, 255, 50);
static const sf::Color SELECT_COLOR(46, 204, 113, 130);
static const sf::Color HINT_QUIET(0, 0, 0, 60);
static const sf::Color HINT_CAPTURE(255, 0, 0, 130);
static const sf::Color LABEL_COLOR(231, 76, 60);

// ========== COLORS - SIDEBAR ==========
static const sf::Color SIDEBAR_PANEL_BLACK(33, 37, 43);
static const sf::Color SIDEBAR_PANEL_WHITE(40, 44, 52);
static const sf::Color TURN_COLOR(0, 229, 255);
static const sf::Color WINNER_GOLD(255, 215, 0);
static const sf::Color MODAL_OVERLAY(0, 0, 0, 180);

// ========== BUTTON COLORS ==========
static const sf::Color BUTTON_COLOR(45, 45, 45);
static const sf::Color BUTTON_HOVER(70, 70, 70);
static const sf::Color BUTTON_ACTIVE(100, 150, 200);

// ========== AI DIFFICULTY LEVELS ==========
enum class Difficulty {
    EASY = 2,
    MEDIUM = 4,
    HARD = 6,
    IMPOSSIBLE = 8
};

// ========== GAME SETTINGS ==========
static constexpr float DEFAULT_TIME_LIMIT = 600.0f; // 10 minutes
static constexpr int MAX_MOVE_HISTORY = 200;
static constexpr float AI_MOVE_DELAY = 0.3f; // Delay before AI moves
static constexpr int NO_VALID_INDEX = -1;

// ========== THREEFOLD REPETITION & 50-MOVE RULE ==========
static constexpr int THREEFOLD_REPETITION_LIMIT = 3;
static constexpr int FIFTY_MOVE_RULE_LIMIT = 100; // 50 moves by both players

// ========== PROMOTION PIECES ==========
static constexpr int8_t PROMOTION_PIECES[4] = {
    WHITE_QUEEN,  // Default promotion
    WHITE_ROOK,
    WHITE_BISHOP,
    WHITE_KNIGHT
};

static constexpr int NUM_PROMOTION_PIECES = 4;

// ========== GAME STATES ==========
enum class GameState {
    MENU,
    DIFFICULTY_SELECT,
    GAME_MODE_SELECT,
    PLAYING,
    PAUSED,
    GAME_OVER
};

// ========== MOVE UNDO/REDO ==========
static constexpr int MAX_MOVE_STACK = 500;

} // namespace Chess
