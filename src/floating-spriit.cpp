// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// floating-siprit.cpp
//
#include "floating-spirit.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"

namespace halloween
{

    FloatingSpirit::FloatingSpirit()
        : m_texture()
        , m_sprite()
        , m_drifter()
    {}

    void FloatingSpirit::setup(const Context & context)
    {
        m_texture.loadFromFile((context.settings.media_path / "image/spirit.png").string());
        m_texture.setSmooth(true);

        m_sprite.setTexture(m_texture);
        m_sprite.setScale(0.5f, 0.5f);
        m_sprite.setColor(sf::Color(255, 255, 255, 16));

        sf::FloatRect region;
        region.left = m_sprite.getGlobalBounds().width;
        region.top = (m_sprite.getGlobalBounds().height + context.layout.infoRegion().height);
        region.width = (context.layout.mapRegion().width - region.left);
        region.height = ((context.layout.mapRegion().height - region.top) - 100.0f);

        m_drifter.setup(context.random, region, std::make_pair(0.0f, 1.0f));

        m_sprite.setPosition(m_drifter.position());
    }

    void FloatingSpirit::update(const Context & context, const float frameTimeSec)
    {
        m_drifter.update(context.random, frameTimeSec);
        m_sprite.setPosition(m_drifter.position());
    }

    void FloatingSpirit::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        states.blendMode = sf::BlendAdd;
        target.draw(m_sprite, states);
    }

} // namespace halloween
