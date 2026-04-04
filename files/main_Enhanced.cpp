#include "ChessBoard_Enhanced.hpp"
#include "ChessTests.hpp"
#include <iostream>
#include <string>

// ============================================================================
// MAIN GAME LOOP WITH ALL FEATURES
// ============================================================================

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        bool runTests = false;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--test" || arg == "-t") {
                runTests = true;
            }
        }

        // Run tests if requested
        if (runTests) {
            std::cout << "\n>>> Running Chess Engine Unit Tests <<<\n";
            ChessTests::runAllTests();
            return ChessTests::TestRunner::getInstance().allPassed() ? 0 : 1;
        }

        // Create game window
        sf::RenderWindow window(
            sf::VideoMode(
                static_cast<unsigned int>(Chess::WINDOW_WIDTH),
                static_cast<unsigned int>(Chess::WINDOW_HEIGHT)
            ),
            "E2-E4 Chess Engine v2.0",
            sf::Style::Close
        );
        window.setFramerateLimit(60);

        std::cout << "\n========================================\n";
        std::cout << "  E2-E4 CHESS ENGINE v2.0\n";
        std::cout << "========================================\n";
        std::cout << "Features:\n";
        std::cout << "  ✓ Pawn Promotion Dialog\n";
        std::cout << "  ✓ Stalemate Detection\n";
        std::cout << "  ✓ Undo/Redo Functionality\n";
        std::cout << "  ✓ Difficulty Settings (Easy/Medium/Hard/Impossible)\n";
        std::cout << "  ✓ Game Statistics & Analysis\n";
        std::cout << "  ✓ Threefold Repetition Detection\n";
        std::cout << "  ✓ 50-Move Rule Detection\n";
        std::cout << "  ✓ AI Engine (Separate Class)\n";
        std::cout << "  ✓ Network Framework (Beta)\n";
        std::cout << "  ✓ Unit Tests Available\n";
        std::cout << "========================================\n\n";
        std::cout << "Controls:\n";
        std::cout << "  - Click to select/move pieces\n";
        std::cout << "  - Press 'U' to undo move\n";
        std::cout << "  - Press 'R' to redo move\n";
        std::cout << "  - Press 'P' to pause/resume\n";
        std::cout << "  - Press 'ESC' to return to menu\n";
        std::cout << "========================================\n\n";

        // Initialize game board
        ChessBoard board;
        sf::Clock deltaClock;

        // Main game loop
        while (window.isOpen()) {
            // Calculate delta time
            float deltaTime = deltaClock.restart().asSeconds();
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            // Process events
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (event.type == sf::Event::MouseButtonPressed && 
                    event.mouseButton.button == sf::Mouse::Left) {
                    board.handleClick(mousePos);
                }

                if (event.type == sf::Event::MouseMoved) {
                    board.handleHover(mousePos);
                }

                // Keyboard shortcuts
                if (event.type == sf::Event::KeyPressed) {
                    switch (event.key.code) {
                        case sf::Keyboard::U:
                            if (board.getState() == Chess::GameState::PLAYING) {
                                board.undoMove();
                            }
                            break;

                        case sf::Keyboard::R:
                            if (board.getState() == Chess::GameState::PLAYING) {
                                board.redoMove();
                            }
                            break;

                        case sf::Keyboard::P:
                            if (board.getState() == Chess::GameState::PLAYING) {
                                if (board.getIsPaused()) {
                                    board.resumeGame();
                                } else {
                                    board.pauseGame();
                                }
                            }
                            break;

                        case sf::Keyboard::Escape:
                            board.setGameState(Chess::GameState::MENU);
                            break;

                        case sf::Keyboard::Num1:
                            // Quick difficulty select (Easy)
                            if (board.getState() == Chess::GameState::DIFFICULTY_SELECT) {
                                board.setAIDifficulty(Chess::Difficulty::EASY);
                                board.setGameState(Chess::GameState::PLAYING);
                                board.resetGame();
                            }
                            break;

                        case sf::Keyboard::Num2:
                            // Medium
                            if (board.getState() == Chess::GameState::DIFFICULTY_SELECT) {
                                board.setAIDifficulty(Chess::Difficulty::MEDIUM);
                                board.setGameState(Chess::GameState::PLAYING);
                                board.resetGame();
                            }
                            break;

                        case sf::Keyboard::Num3:
                            // Hard
                            if (board.getState() == Chess::GameState::DIFFICULTY_SELECT) {
                                board.setAIDifficulty(Chess::Difficulty::HARD);
                                board.setGameState(Chess::GameState::PLAYING);
                                board.resetGame();
                            }
                            break;

                        case sf::Keyboard::Num4:
                            // Impossible
                            if (board.getState() == Chess::GameState::DIFFICULTY_SELECT) {
                                board.setAIDifficulty(Chess::Difficulty::IMPOSSIBLE);
                                board.setGameState(Chess::GameState::PLAYING);
                                board.resetGame();
                            }
                            break;

                        default:
                            break;
                    }
                }
            }

            // Update game state
            board.update(deltaTime);

            // Handle AI moves
            if (board.getIsAiGame() && 
                !board.getIsWhiteTurn() && 
                !board.getIsCheckmate() && 
                !board.getIsStaleMate() &&
                !board.getIsPromoting() &&
                !board.getIsPaused()) {
                board.aiMove();
            }

            // Render
            window.clear(sf::Color(30, 33, 35));
            board.draw(window);
            window.display();
        }

        // Game ended - finalize statistics
        board.finalizeGameStats();
        const GameStatistics& stats = board.getGameStats();
        
        std::cout << "\n========================================\n";
        std::cout << "Game Statistics:\n";
        std::cout << "========================================\n";
        std::cout << "Duration: " << stats.getDurationString() << "\n";
        std::cout << "Total Moves: " << stats.totalMoves << "\n";
        std::cout << "Result: " << stats.getResultString() << "\n";
        std::cout << "White Score: " << stats.whiteScore << "\n";
        std::cout << "Black Score: " << stats.blackScore << "\n";
        std::cout << "========================================\n\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}
