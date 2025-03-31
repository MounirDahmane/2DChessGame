#include <SFML/Window.hpp>

int main(){
    sf::Window window(sf::VideoMode({800, 800}), "Chess", sf::Style::Default, sf::State::Windowed);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }
    }
    
}