#ifndef PAUSE_SCREEN_HPP_INCLUDED
#define PAUSE_SCREEN_HPP_INCLUDED
//
// pause-screen.hpp
//
#include "resources.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{

    class PauseScreen
    {
      public:
        PauseScreen();

        void setup(const sf::Vector2u & t_windowSize, const Resources & t_media);
        void update(const sf::Window & t_window);
        void draw(sf::RenderTarget & t_target, sf::RenderStates t_states);

      private:
        bool m_isSupported;
        sf::Texture m_texture;
        sf::Sprite m_sprite;
        sf::Text m_text;
    };

} // namespace halloween

#endif // PAUSE_SCREEN_HPP_INCLUDED
