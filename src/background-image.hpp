#ifndef BACKGROUND_IMAGE_HPP_INCLUDED
#define BACKGROUND_IMAGE_HPP_INCLUDED
//
// background-image.hpp
//
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

//

namespace sf
{
    class RenderTarget;
    struct RenderStates;
} // namespace sf

//

namespace halloween
{

    struct Context;

    //

    class BackgroundImage
    {
      public:
        BackgroundImage();

        void setup(const Context & t_context, const int t_backgroundImageNumber);
        void draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const;

      private:
        sf::Texture m_texture;
        sf::Sprite m_sprite;
    };

} // namespace halloween

#endif // BACKGROUND_IMAGE_HPP_INCLUDED
