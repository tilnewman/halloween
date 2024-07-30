// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "pause-screen.hpp"

#include "check-macros.hpp"
#include "color-range.hpp"
#include "sfml-util.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    PauseScreen::PauseScreen()
        : m_isSupported(false)
        , m_texture()
        , m_sprite()
        , m_text()
    {}

    void PauseScreen::setup(const sf::Vector2u & windowSize, const Resources & media)
    {
        m_isSupported = m_texture.create(windowSize.x, windowSize.y);

        M_CHECK_LOG(
            m_isSupported, "Your video card sucks so the pause screen won't look very good.");

        m_text.setFont(media.font);
        m_text.setString("PAUSED");
        m_text.setCharacterSize(99);

        const sf::Vector2f windowSizeF{ windowSize };
        sf::FloatRect textRect;
        textRect.left = 0.0f;
        textRect.width = windowSizeF.x;
        textRect.height = (windowSizeF.y / 8.0f);
        textRect.top = ((windowSizeF.y * 0.3f) - (textRect.height * 0.5f));
        util::fitAndCenterInside(m_text, textRect);
    }

    void PauseScreen::update(const sf::Window & WINDOW)
    {
        if (!m_isSupported)
        {
            return;
        }

        m_texture.update(WINDOW);

        sf::Image image = m_texture.copyToImage();

        for (unsigned int y(0); y < image.getSize().y; ++y)
        {
            for (unsigned int x(0); x < image.getSize().x; ++x)
            {
                // brighten all colors
                const auto color{ image.getPixel(x, y) + sf::Color(40, 40, 40) };

                // convert to grayscale
                const float lumen = colors::brightness::Hsl(color);
                const auto grayValue = static_cast<sf::Uint8>(util::mapRatioTo(lumen, 0, 255));

                // reduce the number of colors
                const auto reducedValue = static_cast<sf::Uint8>((grayValue / 35) * 35);

                image.setPixel(x, y, sf::Color(reducedValue, reducedValue, reducedValue));
            }
        }

        m_texture.loadFromImage(image);
        m_sprite.setTexture(m_texture);

        // other colors to try:
        // paper/sepia-ish: 235, 200, 140
        // dark purple: 88,30,157
        m_sprite.setColor(sf::Color(120, 110, 215));
    }

    void PauseScreen::draw(sf::RenderTarget & target)
    {
        target.draw(m_sprite);
        target.draw(m_text);
    }

} // namespace halloween
