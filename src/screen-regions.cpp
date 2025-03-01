// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// screen-regions.cpp
//
#include "screen-regions.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "resources.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace halloween
{

    ScreenRegions::ScreenRegions()
        : m_wholeSize()
        , m_wholeRegion()
        , m_mapRegion()
        , m_infoRegion()
    {}

    void ScreenRegions::setup(const sf::Vector2u & windowSize)
    {
        m_wholeSize = { static_cast<float>(windowSize.x), static_cast<float>(windowSize.y) };
        m_wholeRegion = { { 0.0f, 0.0f }, m_wholeSize };

        m_infoRegion.position.x = 0.0f;
        m_infoRegion.position.y = 0.0f;
        m_infoRegion.size.x = m_wholeRegion.size.x;
        m_infoRegion.size.y = (m_wholeRegion.size.y * 0.075f);
        util::floor(m_infoRegion);

        m_mapRegion.position.x = 0.0f;
        m_mapRegion.position.y = (m_infoRegion.size.y + 1.0f);
        m_mapRegion.size.x = m_wholeRegion.size.x;
        m_mapRegion.size.y = (m_wholeRegion.size.y - m_mapRegion.position.y);
        util::floor(m_mapRegion);
    }

} // namespace halloween
