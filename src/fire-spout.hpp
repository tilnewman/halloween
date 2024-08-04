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
            : spout_sprite()
            , fire_sprite()
        {}

        sf::Sprite spout_sprite;
        sf::Sprite fire_sprite;
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
        float m_elapsedTimeSec;
        std::vector<FireSpout> m_fireSpouts;
        std::vector<sf::Texture> m_fireTextures;
        std::size_t m_fireTextureIndex;
        bool is_spurting;
    };

} // namespace halloween

#endif // FIRESPOUT_HPP_INCLUDED
