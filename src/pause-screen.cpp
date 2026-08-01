// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "pause-screen.hpp"

#include "check-macros.hpp"
#include "color-range.hpp"
#include "context.hpp"
#include "fonts.hpp"
#include "settings.hpp"
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
        m_text = t_context.fonts.makeText(120, "PAUSED", sf::Color(220, 220, 220));

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
        m_sprite.setTexture(m_texture, true);
        m_sprite.setColor(t_context.settings.pause_screen_color);
    }

    void PauseScreen::draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(m_sprite, t_states);
        t_target.draw(m_text, t_states);
    }

} // namespace halloween
