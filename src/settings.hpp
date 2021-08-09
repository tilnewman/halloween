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
        const float frame_rate = 60.0f;
        const std::filesystem::path media_path = "./media";
        const sf::Vector2u target_res = { 1920, 1080 };
    };

} // namespace halloween

#endif // SETTINGS_HPP_INCLUDED
