#include "ChessBoard.hpp"

#include <string>
#include <vector>

enum class GameState
{
    Menu,
    Playing,
    Quit
};

struct MenuButton
{
    sf::RectangleShape shape;
    sf::Text text;

    MenuButton(std::string label, sf::Vector2f pos, sf::Font &font)
    {
        shape.setSize({300, 60});
        shape.setPosition(pos);
        shape.setFillColor(sf::Color(60, 63, 65));
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::White);

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        // Center text
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(
            textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        text.setPosition(pos.x + 150, pos.y + 30);
    }

    bool isClicked(sf::Vector2i mousePos)
    {
        return shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(shape);
        window.draw(text);
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 900), "E2-E4 Chess Engine", sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    font.loadFromFile("assets/fonts/arial.ttf");

    GameState state = GameState::Menu;
    ChessBoard board;
    sf::Clock deltaClock;

    // --- Create Menu Buttons ---
    MenuButton btnPvP("Local 1 vs 1", {450, 300}, font);
    MenuButton btnPvAI("Play vs AI", {450, 400}, font); // New Button
    MenuButton btnQuit("Exit Game", {450, 500}, font);

    while (window.isOpen())
    {
        float dt = deltaClock.restart().asSeconds();
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (state == GameState::Menu)
            {
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left)
                {
                    if (btnPvP.isClicked(mousePos))
                    {
                        board.setAiMode(false);
                        state = GameState::Playing;
                    }
                    if (btnPvAI.isClicked(mousePos))
                    {
                        board.setAiMode(true);
                        state = GameState::Playing;
                    }
                    if (btnQuit.isClicked(mousePos))
                        window.close();
                }
            }
            else if (state == GameState::Playing)
            {
                if (event.type == sf::Event::MouseMoved)
                    board.handleHover(mousePos);
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left)
                {
                    board.handleClick(mousePos);
                }
            }
        }

        window.clear(sf::Color(30, 33, 35));

        if (state == GameState::Menu)
        {
            btnPvP.draw(window);
            btnPvAI.draw(window);
            btnQuit.draw(window);
        }
        else if (state == GameState::Playing)
        {
            board.update(dt);
            board.draw(window);

            // --- AI LOGIC TRIGGER ---
            // If it's AI mode and Black's turn, and no one is choosing a promotion piece
            if (board.getIsAiGame() && !board.getIsWhiteTurn() && !board.getIsCheckmate() &&
                !board.getIsPromoting())
            {
                // Optional: Add a tiny delay here if the AI moves too fast!
                board.aiMove();
            }
        }

        window.display();
    }
    return 0;
}