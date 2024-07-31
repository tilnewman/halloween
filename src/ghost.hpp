#ifndef GHOST_HPP_INCLUDED
#define GHOST_HPP_INCLUDED
//
// ghost.hpp
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

    struct Ghost
    {
        Ghost(
            const Context & context,
            const sf::Texture & texture,
            const float speed,
            const sf::Vector2f & position);

        bool is_alive;
        float speed;
        sf::Sprite sprite;
    };

    //

    class Ghosts
    {
      public:
        Ghosts();

        void setup(const Context & context);
        void clear();
        void update(const Context &, const float frameTimeSec);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;
        void move(const sf::Vector2f & move);

      private:
        sf::Texture m_texture1;
        sf::Texture m_texture2;
        sf::Texture m_texture3;
        std::vector<Ghost> m_ghosts;
    };

} // namespace halloween

#endif // GHOST_HPP_INCLUDED
