#ifndef SLIME_HPP_INCLUDED
#define SLIME_HPP_INCLUDED
//
// slime.hpp
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

    struct Slime
    {
        Slime(const bool isMovingLeft, const sf::FloatRect & range, const float spd)
            : is_alive(true)
            , is_moving_left(isMovingLeft)
            , texture_index(0)
            , rect(range)
            , speed(spd)
            , sprite()
        {}

        bool is_alive;
        bool is_moving_left;
        std::size_t texture_index;
        sf::FloatRect rect;
        float speed;
        sf::Sprite sprite;
    };

    //

    struct SlimeDeathAnim
    {
        SlimeDeathAnim(const sf::Sprite & spr)
            : is_visible(true)
            , sprite(spr)
        {}

        bool is_visible;
        sf::Sprite sprite;
    };

    //

    class Slimes
    {
      public:
        Slimes();

        void setup(const Settings & settings);
        void add(const Context & context, const sf::FloatRect & rect);
        void clear();
        void update(const float frameTimeSec);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;
        void move(const sf::Vector2f & move);

        bool doesCollideWithAny(const sf::FloatRect & rect) const;
        bool attack(const sf::FloatRect & attackRect);

      private:
        std::vector<sf::Texture> m_textures;
        std::vector<sf::FloatRect> m_rects;
        std::vector<Slime> m_slimes;
        float m_timePerTextureSec;
        float m_elapsedTimeSec;
        std::size_t m_textureCount;
        std::vector<SlimeDeathAnim> m_deathAnims;
    };

} // namespace halloween

#endif // SLIME_HPP_INCLUDED
