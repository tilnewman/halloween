// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// spider.cpp
//
#include "spider.hpp"

#include "context.hpp"
#include "random.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"

namespace halloween
{
    Spider::Spider(const Context & t_context, const sf::FloatRect & t_rect)
        : m_anim{ SpiderAnim::Idle }
        , m_type{ t_context.random.from({ SpiderType::Mom, SpiderType::Dad, SpiderType::Child }) }
        , m_webSprite{ t_context.spider_textures.webTexture() }
        , m_spiderSprite{ util::SfmlDefaults::instance().texture() }
        , m_animElapsedTimeSec{ 0.0f }
        , m_frameIndex{ 0 }
        , m_hitPoints{ 2 }
        , m_rect{ t_rect }
    {
        //
        util::setOriginToCenter(m_webSprite);
        util::fitAndCenterInside(m_webSprite, m_rect);
        m_webSprite.setPosition(util::center(m_rect));
        m_webSprite.setRotation(sf::degrees(t_context.random.fromTo(0.0f, 360.0f)));
        m_webSprite.setColor(sf::Color(255, 255, 255, 127));

        //
        m_spiderSprite.setTexture(t_context.spider_textures.textures(m_type, m_anim).at(0), true);
        util::setOriginToCenter(m_spiderSprite);
        util::fitAndCenterInside(m_spiderSprite, m_rect);
        m_spiderSprite.setPosition(util::center(m_rect));

        if (t_context.random.boolean())
        {
            m_spiderSprite.scale({ -1.0f, 1.0f });
        }
    }

    const sf::FloatRect Spider::collisionRect() const { return m_spiderSprite.getGlobalBounds(); }

    const sf::FloatRect Spider::attackRect(const SpiderAnim) const { return collisionRect(); }

    void Spider::update(const Context &, const float) {}

    void Spider::moveWithMap(const sf::Vector2f & t_move)
    {
        m_webSprite.move(t_move);
        m_spiderSprite.move(t_move);
        m_rect.position += t_move;
    }

    bool Spider::doesAvatarCollideWithAnyAndDie(const sf::FloatRect &) const { return false; }

    void Spider::hit(const Context &) {}

    void Spider::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        if (t_context.layout.wholeRegion().findIntersection(m_webSprite.getGlobalBounds()))
        {
            t_target.draw(m_webSprite, t_states);
            t_target.draw(m_spiderSprite, t_states);
        }
    }

} // namespace halloween
