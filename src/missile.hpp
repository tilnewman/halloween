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
        Missile(const sf::Vector2f & pos, const bool isMovingRight)
            : is_alive(true)
            , is_moving_right(isMovingRight)
            , position(pos)
        {}

        bool is_alive;
        bool is_moving_right;
        sf::Vector2f position;
    };

    //

    class Missiles
    {
      public:
        Missiles();

        void setup(const Settings & settings);
        void add(const sf::Vector2f & position, const bool isMovingRight);
        void update(Context &, const float frameTimeSec);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;

      private:
        sf::Texture m_texture;
        sf::Vector2f m_velocity;
        sf::Vector2f m_scale;
        std::vector<Missile> m_missiles;
    };

} // namespace halloween

#endif // MISSLE_HPP_INCLUDED
