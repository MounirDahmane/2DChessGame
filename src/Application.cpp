/**
 * @file Application.cpp
 * @brief Implementation of the Application class handling the main game loop.
 */

#include "Application.hpp"
#include "Constants.hpp"

Application::Application() 
    : window(sf::VideoMode(static_cast<unsigned int>(Chess::WINDOW_WIDTH), 
                           static_cast<unsigned int>(Chess::WINDOW_HEIGHT)), 
             "E2-E4 Chess Engine", sf::Style::Close) 
{
    window.setFramerateLimit(60);
}

void Application::run() {
    sf::Clock deltaClock;

    while (window.isOpen()) {
        // Safely exit the application if requested by the in-game menu
        if (board.getShouldExit()) {
            window.close();
            break;
        }

        float deltaTime = deltaClock.restart().asSeconds();

        processEvents();
        update(deltaTime);
        render();
    }
}

void Application::processEvents() {
    sf::Event event;
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } 
        else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            board.handleClick(mousePos);
        } 
        else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            board.handleRelease(mousePos);
        } 
        else if (event.type == sf::Event::MouseMoved) {
            board.handleHover(mousePos);
        }
    }
}

void Application::update(float dt) {
    // Propagate the delta time update to the chess engine (handles timers and async AI)
    board.update(dt);
}

void Application::render() {
    // Clear the background with the dark sidebar theme color
    window.clear(sf::Color(30, 33, 35));
    board.draw(window);
    window.display();
}