// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// info-region.cpp
//
#include "info-region.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "resources.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"

#include <sstream>

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    InfoRegion::InfoRegion()
        : m_score(0)
        , m_visibleScore(0)
        , m_elapsedTimeSec(0.0f)
        , m_timePerScoreUpdate(0.1f)
        , m_lives(0)
        , m_text()
        , m_region()
        , m_bgVerts()
    {}

    void InfoRegion::update(Context & context, const float frameTimeSec)
    {
        if (m_score == m_visibleScore)
        {
            return;
        }

        m_elapsedTimeSec += frameTimeSec;
        if (m_elapsedTimeSec > m_timePerScoreUpdate)
        {
            m_elapsedTimeSec -= m_timePerScoreUpdate;

            if ((m_score - m_visibleScore) > 10)
            {
                m_visibleScore = m_score;
                context.audio.play("bell", 0.5f);
            }
            else
            {
                ++m_visibleScore;
                context.audio.play("bell");
            }

            updateText();
        }
    }

    void InfoRegion::reset(Context & context)
    {
        m_elapsedTimeSec = 0.0f;
        m_score = 0;
        m_visibleScore = 0;
        m_lives = context.settings.player_lives;
        updateText();
    }

    void InfoRegion::setup(Context & context)
    {
        m_region = context.layout.infoRegion();

        util::appendQuadVerts(context.layout.infoRegion(), m_bgVerts, sf::Color(29, 30, 30));

        m_text.setFont(context.media.font);
        m_text.setCharacterSize(99);
        m_text.setFillColor(sf::Color(52, 64, 65));
        util::setOriginToPosition(m_text);

        reset(context);
    }

    void InfoRegion::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(&m_bgVerts[0], m_bgVerts.size(), sf::Quads, states);
        target.draw(m_text, states);
    }

    void InfoRegion::scoreAdjust(const int adj)
    {
        m_score += adj;
        // updateText();
    }

    void InfoRegion::livesAdjust(const int adj)
    {
        m_lives += adj;
        updateText();
    }

    void InfoRegion::updateText()
    {
        std::ostringstream ss;

        ss << "Lives: " << std::setw(2) << std::setfill('0') << m_lives
           << "      Score: " << std::setw(6) << std::setfill('0') << m_visibleScore;

        m_text.setString(ss.str());
        util::setOriginToPosition(m_text);
        util::fitAndCenterInside(m_text, util::scaleRectInPlaceCopy(m_region, { 1.0f, 0.5f }));
    }

} // namespace halloween
