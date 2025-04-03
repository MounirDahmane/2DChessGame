#include "ChessBoard.hpp" //isotream, vector, sfml/Graphics.hpp, std::array

// save state before closing
// text {numbers and letters} in squares

#define height
#define FRAME_RATE 60

int main()
{
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    // create the window
    sf::RenderWindow window(sf::VideoMode({900, 900}), "My window", sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(FRAME_RATE);

    ChessBoard board;
    
    
    //auto spriteOpt = board.load("./assets/images/Untitled.png");
    //sf::Sprite sprite = *spriteOpt;
    
    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();

            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }
            if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
            {
                //std::cout << "new mouse y: " << mouseMoved->position.y << std::endl;
                //std::cout << "new mouse x: " << mouseMoved->position.x << std::endl;
                board.setColor(mouseMoved->position);
                
            }
            //sf::Vector2i localPosition = sf::Mouse::getPosition(window);
            //if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
            
        }

        // clear the window with black color
        window.clear();

        // draw everything here...
       
        board.draw(window);
        
        // end the current frame
        window.display();
        
    }
    
}

// to pause and resume
        //if (event->is<sf::Event::FocusLost>())
        //    myGame.pause();
        //
        //if (event->is<sf::Event::FocusGained>())
        //    myGame.resume();

        /*if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouseButtonPressed->button == sf::Mouse::Button::Right)
            {
                std::cout << "the right button was pressed" << std::endl;
                std::cout << "mouse x: " << mouseButtonPressed->position.x << std::endl;
                std::cout << "mouse y: " << mouseButtonPressed->position.y << std::endl;
            }
        }*/

        // modify the linking and including