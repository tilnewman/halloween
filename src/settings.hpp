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
        const sf::Vector2u target_screen_res = { 1920, 1080 };

        const std::filesystem::path media_path = "./media";

        const int player_lives = 3;

        // display sizes
        const float tile_scale = 0.75f;
        const float avatar_scale = 0.2f;
        const sf::Vector2f dart_scale = { 0.333f, 0.5f };

        // frames per second
        const float frame_rate = 60;
        const bool will_display_fps = false;
        const bool will_display_fps_graph = false;

        // movement speeds and accelerations
        const sf::Vector2f gravity_acc = { 0.0f, 25.0f };
        const float jump_acc = 900.0f;
        const float walk_acc = 15.0f;
        const float walk_speed_limit = 5.0f;
        const float dart_speed = 1000.0f;
        const float walk_speed_attack_reduction_ratio = 0.8f;
        const float walk_speed_throw_reduction_ratio = 0.8f;

        // delay after dying so the player can see how they died
        const float death_delay_sec = 4.0f;

        // this will cause minor artifacts and I can't tell the diff in quality
        const bool will_smooth_tile_textures = false;
    };

} // namespace halloween

#endif // SETTINGS_HPP_INCLUDED
