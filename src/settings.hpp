#ifndef SETTINGS_HPP_INCLUDED
#define SETTINGS_HPP_INCLUDED
//
// settings.hpp
//
#include <filesystem>

#include <SFML/System/Vector2.hpp>

namespace halloween
{

    struct Settings
    {
        const float avatar_scale = 0.2f;
        const float tile_scale = 0.75f;
        const std::size_t frame_rate = 60;
        const std::filesystem::path media_path = "./media";
        const sf::Vector2u target_res = { 1920, 1080 };
        const bool will_display_fps = true;
        const bool will_display_fps_graph = false;
    };

} // namespace halloween

#endif // SETTINGS_HPP_INCLUDED
