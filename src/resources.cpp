// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// resources.cpp
//
#include "resources.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "texture-loader.hpp"

namespace halloween
{

    Resources::Resources()
        : m_font{}
    {}

    void Resources::setup(const Settings & t_settings)
    {
        // font
        const std::string fontPathStr{
            (t_settings.media_path / "font" / "mops-antiqua.ttf").string()
        };

        const bool fontLoadSuccess{ m_font.openFromFile(fontPathStr) };
        M_CHECK(fontLoadSuccess, "Failed to load font: " << fontPathStr);
    }

    const sf::Text Resources::makeText(
        const unsigned int t_charSize,
        const std::string & t_message,
        const sf::Color & t_color) const
    {
        sf::Text text(m_font, t_message, t_charSize);
        text.setFillColor(t_color);
        util::setOriginToPosition(text);
        return text;
    }

} // namespace halloween
