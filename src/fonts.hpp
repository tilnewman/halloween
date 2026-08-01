#ifndef FONTS_HPP_INCLUDED
#define FONTS_HPP_INCLUDED
//
// resources.hpp
//
#include "tileset.hpp"

#include <string>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

namespace halloween
{

    struct Settings;

    //

    enum class Font
    {
        Title,
        General
    };

    //

    class FontManager
    {
      public:
        FontManager();

        void setup(const Settings & t_settings);

        const sf::Text makeText(
            const Font t_font,
            const unsigned int t_charSize,
            const std::string & t_message,
            const sf::Color & t_color = sf::Color::White) const;

      private:
        sf::Font m_titleFont;
        sf::Font m_generalFont;
    };

} // namespace halloween

#endif // FONTS_HPP_INCLUDED
