// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// fonts.cpp
//
#include "fonts.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"

namespace halloween
{

    FontManager::FontManager()
        : m_titleFont{}
        , m_generalFont{}
    {}

    void FontManager::setup(const Settings & t_settings)
    {
        const std::string titlePath{
            (t_settings.media_path / "font" / "mops-antiqua.ttf").string()
        };

        const bool titleSuccess{ m_titleFont.openFromFile(titlePath) };
        M_CHECK(titleSuccess, "Failed to load the title font: " << titlePath);

        const std::string generalPath{
            (t_settings.media_path / "font" / "gentium-plus.ttf").string()
        };

        const bool generalSuccess{ m_generalFont.openFromFile(generalPath) };
        M_CHECK(generalSuccess, "Failed to load the general font: " << generalPath);
    }

    const sf::Text FontManager::makeText(
        const Font t_font,
        const unsigned int t_charSize,
        const std::string & t_message,
        const sf::Color & t_color) const
    {
        sf::Text text = [&]() {
            if (Font::Title == t_font)
            {
                return sf::Text(m_titleFont, t_message, t_charSize);
            }
            else
            {
                return sf::Text(m_generalFont, t_message, t_charSize);
            }
        }();

        text.setFillColor(t_color);
        util::setOriginToPosition(text);
        return text;
    }

    const sf::Vector2f FontManager::makeExtent(const Font t_font, const unsigned t_size) const
    {
        sf::Vector2f size;
        size.x = makeText(t_font, t_size, "M").getGlobalBounds().size.x;
        size.y = makeText(t_font, t_size, "|g").getGlobalBounds().size.y;
        return size;
    }

} // namespace halloween
