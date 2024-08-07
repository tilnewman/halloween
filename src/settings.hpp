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
        int player_lives = 3;
        std::size_t boss_hit_points = 5;

        // see main.cpp for where this is really set
        std::filesystem::path media_path = { "./media" };

        sf::Vector2u target_screen_res = { 1920, 1080 };

        // display sizes
        float tile_scale = 0.75f;
        float avatar_scale = 0.8f;
        sf::Vector2f dart_scale = { 0.333f, 0.5f };
        sf::Vector2f ghost_scale = { 0.5f, 0.5f };
        sf::Vector2f spiked_ball_scale = { 2.0f, 2.0f };

        // frames per second
        float frame_rate = 60;
        bool will_display_fps = false;
        bool will_display_fps_graph = false;

        // movement speeds and accelerations
        sf::Vector2f gravity_acc = { 0.0f, 25.0f };
        float jump_acc = 900.0f;
        float walk_acc = 15.0f;
        float walk_speed_limit = 5.0f;
        float dart_speed = 1000.0f;
        float walk_speed_attack_reduction_ratio = 0.8f;
        float walk_speed_throw_reduction_ratio = 0.8f;

        // delay after dying so the player can see how they died
        float death_delay_sec = 4.0f;

        // this will cause minor artifacts and I can't tell the diff in quality anyway
        bool will_smooth_tile_textures = false;

        bool will_show_coin_animations = true;

        int starting_dart_count = 3;

        int kill_slime_score = 10;
        int kill_bat_score = 8;
        int kill_boss_score = 99;

        float spiked_ball_speed = 2.0f;
    };

} // namespace halloween

#endif // SETTINGS_HPP_INCLUDED
