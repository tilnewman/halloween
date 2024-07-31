// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "check-macros.hpp"
#include "game-loop.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>

int main(const int argc, const char * const argv[])
{
    try
    {
        halloween::Settings settings;
        if (argc > 1)
        {
            settings.media_path =
                std::filesystem::current_path() / std::filesystem::path{ argv[1] };
        }
        else
        {
            settings.media_path = std::filesystem::current_path() / "media";
        }

        settings.media_path = std::filesystem::canonical(settings.media_path);

        M_CHECK(
            std::filesystem::exists(settings.media_path),
            "Error:  The media path does not exist:"
                << settings.media_path
                << "\nPut the media path on the command line or put the 'media' folder here.");

        std::cout << "Using media folder: " << settings.media_path.string() << std::endl;

        halloween::GameLoop gameLoop(settings);
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
