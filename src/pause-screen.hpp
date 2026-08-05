#ifndef PAUSE_SCREEN_HPP_INCLUDED
#define PAUSE_SCREEN_HPP_INCLUDED
//
// pause-screen.hpp
//
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sf
{
    class RenderTarget;
}

namespace halloween
{
    struct Context;

    //

    class PauseScreen
    {
      public:
        PauseScreen();

        void setup(const Context & t_context);
        void draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const;

      private:
        sf::Texture m_texture;
        sf::Sprite m_sprite;
        sf::Text m_text;
    };

} // namespace halloween

#endif // PAUSE_SCREEN_HPP_INCLUDED
