#pragma once

#include <SFML/Graphics.hpp>
#include "ChessBoard.hpp"

/**
 * @class Application
 * @brief Manages the OS window, game loop, and event polling.
 * * This class encapsulates the SFML window and the main Game Loop. 
 * It separates the OS-level concerns (rendering, events, delta time) 
 * from the core chess logic.
 */
class Application {
public:
    /**
     * @brief Initializes the application window and game settings.
     */
    Application();

    /**
     * @brief Starts the main game loop.
     * * Continuously processes events, updates game logic, and renders 
     * the screen until the window is closed.
     */
    void run();

private:
    /**
     * @brief Polls and routes SFML window and mouse events to the engine.
     */
    void processEvents();

    /**
     * @brief Updates the game state based on elapsed time.
     * @param dt Delta time (seconds) since the last frame.
     */
    void update(float dt);

    /**
     * @brief Clears the window, draws the board, and displays the frame.
     */
    void render();

    sf::RenderWindow window; ///< The main SFML application window.
    ChessBoard board;        ///< The core chess engine and board renderer.
};