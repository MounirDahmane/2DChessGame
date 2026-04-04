#include "Application.hpp"

#include <iostream>

int main()
{
    try
    {
        Application app;
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown fatal error occurred\n";
        return 1;
    }
    return 0;
}