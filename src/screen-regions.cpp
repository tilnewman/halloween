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
#include "util.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace halloween
{

    ScreenRegions::ScreenRegions()
        : m_wholeSize()
        , m_wholeRegion()
        , m_mapRegion()
        , m_infoRegion()
    {}

    void ScreenRegions::setup(Context & context, const sf::Vector2u & WINDOW_SIZE)
    {
        m_wholeSize = { static_cast<float>(WINDOW_SIZE.x), static_cast<float>(WINDOW_SIZE.y) };
        m_wholeRegion = { { 0.0f, 0.0f }, m_wholeSize };

        // calc position of both the map and info regions
        util::fitAndCenterInside(context.media.bg_sprite, m_wholeRegion);

        context.media.bg_sprite.setPosition(
            0.0f, m_wholeSize.y - context.media.bg_sprite.getGlobalBounds().height);

        m_mapRegion = context.media.bg_sprite.getGlobalBounds();

        m_infoRegion.left = 0.0f;
        m_infoRegion.top = 0.0f;
        m_infoRegion.width = m_wholeSize.x;
        m_infoRegion.height = context.media.bg_sprite.getPosition().y;
    }

} // namespace halloween
