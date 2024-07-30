#ifndef PAUSE_SCREEN_HPP_INCLUDED
#define PAUSE_SCREEN_HPP_INCLUDED
//
// pause-screen.hpp
//
#include "resources.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{

    class PauseScreen
    {
      public:
        PauseScreen();

        void setup(const sf::Vector2u & WINDOW_SIZE, const Resources & MEDIA);
        void update(const sf::Window & WINDOW);
        void draw(sf::RenderTarget & target);

      private:
        bool m_isSupported;
        sf::Texture m_texture;
        sf::Sprite m_sprite;
        sf::Text m_text;
    };

} // namespace halloween

#endif // PAUSE_SCREEN_HPP_INCLUDED
