#ifndef RESOURCES_HPP_INCLUDED
#define RESOURCES_HPP_INCLUDED
//
// resources.hpp
//
#include "tileset.hpp"

#include <string>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{

    struct Context;
    struct Settings;

    //

    class Resources
    {
      public:
        Resources();

        void setup(const Settings & t_settings);

        const sf::Text makeText(
            const unsigned int t_charSize,
            const std::string & t_message,
            const sf::Color & t_color) const;

      private:
        sf::Font m_font;
    };

} // namespace halloween

#endif // RESOURCES_HPP_INCLUDED
