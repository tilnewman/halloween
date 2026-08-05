#ifndef MISSLE_HPP_INCLUDED
#define MISSLE_HPP_INCLUDED
//
// missle.hpp
//
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{
    struct Context;
    struct Settings;

    //

    struct Missile
    {
        Missile(const bool t_isMovingRight, const sf::Texture & t_texture)
            : is_alive(true)
            , is_moving_right(t_isMovingRight)
            , sprite(t_texture)
        {}

        bool is_alive;
        bool is_moving_right;
        sf::Sprite sprite;
    };

    //

    class Missiles
    {
      public:
        Missiles();

        void setup(const Settings & t_settings);
        void add(const sf::Vector2f & t_position, const bool t_isMovingRight);
        void update(const Context & t_context, const float t_frameTimeSec);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

        constexpr void clear() noexcept { m_missiles.clear(); }

      private:
        sf::Texture m_texture;
        sf::Vector2f m_velocity;
        sf::Vector2f m_scale;
        std::vector<Missile> m_missiles;
    };

} // namespace halloween

#endif // MISSLE_HPP_INCLUDED
