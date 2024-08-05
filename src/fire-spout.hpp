#ifndef FIRESPOUT_HPP_INCLUDED
#define FIRESPOUT_HPP_INCLUDED
//
// fire-spout.hpp
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

    struct FireSpout
    {
        FireSpout()
            : is_spurting(false)
            , spout_sprite()
            , fire_sprite()
            , texture_index(0)
            , elapsed_time_sec(0.0f)
            , time_between_spurts_sec(0.0f)
        {}

        bool is_spurting;
        sf::Sprite spout_sprite;
        sf::Sprite fire_sprite;
        std::size_t texture_index;
        float elapsed_time_sec;
        float time_between_spurts_sec;
    };

    //

    class FireSpouts
    {
      public:
        FireSpouts();

        void setup(const Settings & settings);
        void add(const Context & context, const sf::FloatRect & region);
        void clear();
        void update(const float frameTimeSec);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;
        void move(const sf::Vector2f & move);
        bool doesCollideWithAny(const sf::FloatRect & avatarRect) const;
        const std::vector<sf::FloatRect> collisions() const;

      private:
        sf::Texture m_spoutTexture;
        float m_timePerFrame;
        std::vector<FireSpout> m_fireSpouts;
        std::vector<sf::Texture> m_fireTextures;
        float m_timebetweenSpurtsMinSec;
        float m_timebetweenSpurtsMaxSec;
    };

} // namespace halloween

#endif // FIRESPOUT_HPP_INCLUDED
