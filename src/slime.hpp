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
        Slime(const bool isOrange)
            : is_alive(true)
            , is_orange(isOrange)
            , is_moving_left(true)
            , texture_index(0)
            , sprite()
        {}

        bool is_alive;
        bool is_orange;
        bool is_moving_left;
        std::size_t texture_index;
        sf::Sprite sprite;
    };

    //

    class Slimes
    {
      public:
        Slimes();

        void setup(const Settings & settings);

        void clearGreenRects() { m_greenSpawnRects.clear(); }
        void clearOrangeRects() { m_orangeSpawnRects.clear(); }
        void clearSlimes() { m_slimes.clear(); }

        void addGreenRect(const sf::FloatRect & rect) { m_greenSpawnRects.push_back(rect); }
        void addOrangeRect(const sf::FloatRect & rect) { m_orangeSpawnRects.push_back(rect); }

        void update(const Context &, const float frameTimeSec);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;
        void move(const sf::Vector2f & move);

      private:
        std::vector<sf::Texture> m_greenTextures;
        std::vector<sf::Texture> m_orangeTextures;
        std::vector<sf::FloatRect> m_greenSpawnRects;
        std::vector<sf::FloatRect> m_orangeSpawnRects;
        std::vector<Slime> m_slimes;
        float m_timePerTextureSec;
    };

} // namespace halloween

#endif // SLIME_HPP_INCLUDED
