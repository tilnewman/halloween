#ifndef MISSLE_HPP_INCLUDED
#define MISSLE_HPP_INCLUDED
//
// missle.hpp
//
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{
    struct Context;
    struct Settings;

    //
    struct Missile
    {
        Missile(const sf::Vector2f & POSITION, const bool IS_MOVING_RIGHT)
            : is_alive(true)
            , is_moving_right(IS_MOVING_RIGHT)
            , position(POSITION)
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

        void setup(const Settings & SETTINGS);
        void add(const sf::Vector2f & POSITION, const bool IS_MOVING_RIGHT);
        void update(Context &, const float FRAME_TIME_SEC);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;

      private:
        sf::Texture m_texture;
        const sf::Vector2f m_velocity;
        std::vector<Missile> m_missiles;
    };

} // namespace halloween

#endif // MISSLE_HPP_INCLUDED
