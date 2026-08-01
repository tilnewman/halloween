// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// framerate-display.cpp
//
#include "framerate-display.hpp"

#include "context.hpp"
#include "fonts.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"

#include <SFML/Graphics/Text.hpp>

namespace halloween
{

    FramerateDisplay::FramerateDisplay()
        : m_text{ util::SfmlDefaults::instance().font() }
        , m_framesPerSecond{}
        , m_graphDisplayUPtr{}
    {}

    void FramerateDisplay::update(const Context & t_context, const float t_elapsedTimeSec)
    {
        m_framesPerSecond.push_back(static_cast<std::size_t>(std::round(1.0f / t_elapsedTimeSec)));

        if (m_framesPerSecond.size() < static_cast<std::size_t>(t_context.settings.frame_rate))
        {
            return;
        }

        if (t_context.settings.will_display_fps)
        {
            std::string str;
            str = " fps: ";
            str += util::makeStats(m_framesPerSecond).toString();

            m_text = t_context.fonts.makeText(Font::General, 60, str, sf::Color(195, 160, 126));

            util::fitAndCenterInside(
                m_text,
                util::scaleRectInPlaceCopy(t_context.layout.infoRegion(), { 1.0f, 0.375f }));

            m_text.setPosition({ 0.0f, (t_context.layout.wholeSize().y - 50.0f) });
        }

        if (t_context.settings.will_display_fps_graph)
        {
            std::sort(std::begin(m_framesPerSecond), std::end(m_framesPerSecond));

            m_graphDisplayUPtr = std::make_unique<util::GraphDisplay<std::size_t>>(
                m_framesPerSecond, sf::Vector2u{ 500, 200 }, std::uint8_t(64));
        }

        m_framesPerSecond.clear();
    }

    void FramerateDisplay::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        if (t_context.settings.will_display_fps)
        {
            t_target.draw(m_text, t_states);
        }

        if (t_context.settings.will_display_fps_graph && m_graphDisplayUPtr)
        {
            t_target.draw(*m_graphDisplayUPtr, t_states);
        }
    }

} // namespace halloween
