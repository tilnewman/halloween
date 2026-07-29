// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "pause-screen.hpp"

#include "check-macros.hpp"
#include "color-range.hpp"
#include "context.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"
#include "texture-loader.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Image.hpp>

namespace halloween
{

    PauseScreen::PauseScreen()
        : m_texture{}
        , m_sprite{ util::SfmlDefaults::instance().texture() }
        , m_text{ util::SfmlDefaults::instance().font() }
    {}

    void PauseScreen::setup(const Context & t_context)
    {
        m_text.setFont(t_context.media.font);
        m_text.setString("PAUSED");
        m_text.setCharacterSize(99);

        const sf::Vector2f windowSizeF{ t_context.window.getSize() };
        sf::FloatRect textRect;
        textRect.position.x = 0.0f;
        textRect.size.x = windowSizeF.x;
        textRect.size.y = (windowSizeF.y / 8.0f);
        textRect.position.y = ((windowSizeF.y * 0.3f) - (textRect.size.y * 0.5f));
        util::fitAndCenterInside(m_text, textRect);

        if (!m_texture.resize(t_context.window.getSize()))
        {
            M_LOG("Your video card sucks so the pause screen won't look very good.");
            return;
        }

        m_texture.update(t_context.window);
        sf::Image image{ m_texture.copyToImage() };

        for (unsigned int y{ 0 }; y < image.getSize().y; ++y)
        {
            for (unsigned int x{ 0 }; x < image.getSize().x; ++x)
            {
                // brighten all colors
                const auto color{ image.getPixel({ x, y }) + sf::Color(40, 40, 40) };

                // convert to grayscale
                const float lumen{ colors::brightness::Hsl(color) };
                const auto grayValue{ static_cast<std::uint8_t>(util::mapRatioTo(lumen, 0, 255)) };

                // reduce the number of colors
                const auto reducedValue{ static_cast<std::uint8_t>((grayValue / 35) * 35) };

                image.setPixel({ x, y }, sf::Color(reducedValue, reducedValue, reducedValue));
            }
        }

        if (m_texture.loadFromImage(image))
        {
            m_sprite.setTexture(m_texture, true);
            m_sprite.setColor(sf::Color(120, 110, 215)); // TODO move to settings
        }
    }

    void PauseScreen::draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(m_sprite, t_states);
        t_target.draw(m_text, t_states);
    }

} // namespace halloween
