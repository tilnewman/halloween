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

#include <sstream>

namespace halloween
{

    InfoRegion::InfoRegion()
        : m_score(0)
        , m_lives(0)
        , m_text()
        , m_region()
        , m_bgVerts()
    {}

    void InfoRegion::reset()
    {
        m_score = 0;
        m_lives = 3;
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

        reset();
    }

    void InfoRegion::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(&m_bgVerts[0], m_bgVerts.size(), sf::Quads, states);
        target.draw(m_text, states);
    }

    void InfoRegion::scoreAdjust(const int ADJ)
    {
        m_score += ADJ;
        updateText();
    }

    void InfoRegion::livesAdjust(const int ADJ)
    {
        m_lives += ADJ;
        updateText();
    }

    void InfoRegion::updateText()
    {
        std::ostringstream ss;

        ss << "Lives: " << std::setw(2) << std::setfill('0') << m_lives
           << "      Score: " << std::setw(6) << std::setfill('0') << m_score;

        m_text.setString(ss.str());

        util::fitAndCenterInside(m_text, util::scaleRectInPlaceCopy(m_region, { 1.0f, 0.5f }));
    }

} // namespace halloween
