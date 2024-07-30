// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "game-loop.hpp"

#include <iostream>
#include <stdexcept>

int main()
{
    try
    {
        halloween::GameLoop gameLoop;
        gameLoop.play();
    }
    catch (const std::exception & ex)
    {
        std::cerr << "Exception Error: \"" << ex.what() << "\"" << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "Non-Standard Exception Error" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
