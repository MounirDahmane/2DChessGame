#pragma once
#include <SFML/Graphics.hpp>

namespace Chess
{
static constexpr float PIXEL_WIDTH = 100.0f;
static constexpr float OFFSET_X = 50.0f;
static constexpr float OFFSET_Y = 50.0f;
static constexpr float SIDEBAR_X = 900.0f;
static constexpr float SIDEBAR_WIDTH = 300.0f;

static const sf::Color BLACK_SQUARE(181, 136, 99);
static const sf::Color WHITE_SQUARE(240, 217, 181);

static const sf::Color HOVER_COLOR(255, 255, 255, 50);
static const sf::Color SELECT_COLOR(46, 204, 113, 130);
static const sf::Color HINT_QUIET(0, 0, 0, 60);
static const sf::Color HINT_CAPTURE(255, 0, 0, 130);
static const sf::Color LABEL_COLOR(231, 76, 60);

static const sf::Color SIDEBAR_PANEL_BLACK(33, 37, 43);
static const sf::Color SIDEBAR_PANEL_WHITE(40, 44, 52);
static const sf::Color TURN_COLOR(0, 229, 255);
static const sf::Color WINNER_GOLD(255, 215, 0);

// NEW: Promotion & UI Colors
static const sf::Color MODAL_OVERLAY(0, 0, 0, 150);
static const sf::Color LOG_TEXT_COLOR(180, 180, 180);
} // namespace Chess

struct MoveHint
{
    int index;
    bool isCapture;
};
