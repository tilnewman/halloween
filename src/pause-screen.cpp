// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "pause-screen.hpp"

#include "check-macros.hpp"
#include "color-range.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>

namespace halloween
{

    PauseScreen::PauseScreen()
        : m_isSupported(false)
        , m_texture()
        , m_sprite()
        , m_text()
    {}

    void PauseScreen::setup(const sf::Vector2u & WINDOW_SIZE, const Resources & MEDIA)
    {
        m_isSupported = m_texture.create(WINDOW_SIZE.x, WINDOW_SIZE.y);

        M_CHECK_LOG(
            m_isSupported, "Your video card sucks so the pause screen won't look very good.");

        m_text.setFont(MEDIA.font);
        m_text.setString("PAUSED");
        m_text.setCharacterSize(99);

        const sf::Vector2f WINDOW_SIZEF{ WINDOW_SIZE };
        sf::FloatRect textRect;
        textRect.left = 0.0f;
        textRect.width = WINDOW_SIZEF.x;
        textRect.height = (WINDOW_SIZEF.y / 8.0f);
        textRect.top = ((WINDOW_SIZEF.y * 0.3f) - (textRect.height * 0.5f));
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
                const auto COLOR{ image.getPixel(x, y) + sf::Color(40, 40, 40) };

                // convert to grayscale
                const float LUMEN = colors::brightness::Hsl(COLOR);

                // reduce the number of colors
                const auto GRAY_VALUE = static_cast<sf::Uint8>(util::mapRatioTo(LUMEN, 0, 255));
                const auto REDUCED_VALUE = static_cast<sf::Uint8>((GRAY_VALUE / 35) * 35);

                image.setPixel(x, y, sf::Color(REDUCED_VALUE, REDUCED_VALUE, REDUCED_VALUE));
            }
        }

        m_texture.loadFromImage(image);
        m_sprite.setTexture(m_texture);

        // other colors to try:
        // paper/sepia-ish color: 235, 200, 140
        // dark purple: 88,30,157
        m_sprite.setColor(sf::Color(120, 110, 215));
    }

    void PauseScreen::draw(sf::RenderTarget & target)
    {
        target.draw(m_sprite);
        target.draw(m_text);
    }

} // namespace halloween
