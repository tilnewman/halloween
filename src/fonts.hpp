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

    struct TextDetails
    {
        TextDetails()
            : font{ Font::General }
            , size{ 30 }
            , color{ sf::Color::White }
            , style{ sf::Text::Regular }
            , will_center_horiz{ true }
            , will_center_vert{ true }
            , inner_pad_ratio{ 0.0f }
        {}

        TextDetails(
            const Font t_font,
            const unsigned t_size,
            const sf::Color & t_color = sf::Color::White,
            const sf::Text::Style t_style = sf::Text::Regular,
            const bool t_willCenterHoriz = true,
            const bool t_willCenterVert = true,
            const float t_innerPadRatio = 0.0f)
            : font{ t_font }
            , size{ t_size }
            , color{ t_color }
            , style{ t_style }
            , will_center_horiz{ t_willCenterHoriz }
            , will_center_vert{ t_willCenterVert }
            , inner_pad_ratio{ t_innerPadRatio }
        {}

        Font font;
        unsigned size;
        sf::Color color;
        sf::Text::Style style;
        bool will_center_horiz;
        bool will_center_vert;
        float inner_pad_ratio;
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

        const sf::Vector2f makeExtent(const Font t_font, const unsigned t_size) const;

      private:
        sf::Font m_titleFont;
        sf::Font m_generalFont;
    };

} // namespace halloween

#endif // FONTS_HPP_INCLUDED
