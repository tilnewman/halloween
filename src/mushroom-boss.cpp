// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// mushroom-boss.cpp
//
#include "mushroom-boss.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "info-region.hpp"
#include "level.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"

#include <algorithm>

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    MushroomBoss::MushroomBoss()
        : m_state(BossState::Idle)
        , m_idleAnim()
        , m_jumpAnim()
        , m_attackAnim()
        , m_hitAnim()
        , m_shakeAnim()
        , m_deathAnim()
        , m_sprite()
        , m_region()
        , m_isThereABossOnThisLevel(false)
        , m_hasFightBegun(false)
    {}

    void MushroomBoss::setup(const Settings & settings)
    {
        m_idleAnim.setup((settings.media_path / "image/mushroom"), "idle", 31, 0.05f, true);
        m_jumpAnim.setup((settings.media_path / "image/mushroom"), "jump", 31, 0.1f, false);
        m_attackAnim.setup((settings.media_path / "image/mushroom"), "attack", 18, 0.175f, false);
        m_hitAnim.setup((settings.media_path / "image/mushroom"), "hit", 6, 0.175f, false);
        m_shakeAnim.setup((settings.media_path / "image/mushroom"), "shake", 5, 0.15f, false);
        m_deathAnim.setup((settings.media_path / "image/mushroom"), "death", 23, 0.2f, false);

        m_sprite.setTexture(m_idleAnim.texture(), true);
        m_sprite.scale(1.0f, 1.0f);
    }

    void MushroomBoss::add(Context &, const sf::FloatRect & rect)
    {
        m_region = rect;
        m_isThereABossOnThisLevel = true;

        m_sprite.setPosition(
            (util::right(m_region) - m_sprite.getGlobalBounds().width),
            (util::bottom(m_region) - m_sprite.getGlobalBounds().height));
    }

    void MushroomBoss::update(Context &, const float frameTimeSec)
    {
        if (!m_isThereABossOnThisLevel)
        {
            return;
        }

        AvatarAnim & anim{ currentAnim() };
        if (anim.update(frameTimeSec))
        {
            m_sprite.setTexture(anim.texture(), true);
        }

        if (anim.isFinished())
        {
            m_state = BossState::Idle;
            currentAnim().restart();
        }
    }

    void MushroomBoss::draw(
        const Context & context, sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (!m_isThereABossOnThisLevel)
        {
            return;
        }

        if (context.layout.mapRegion().intersects(m_sprite.getGlobalBounds()))
        {
            target.draw(m_sprite, states);

            const auto rects = collisionRects();
            util::drawRectangleShape(target, rects.top, false, sf::Color::Red);
            util::drawRectangleShape(target, rects.bottom, false, sf::Color::Green);
        }
    }

    void MushroomBoss::moveWithMap(const sf::Vector2f & move)
    {
        if (!m_isThereABossOnThisLevel)
        {
            return;
        }

        m_sprite.move(move);
        m_region.left += move.x;
    }

    void MushroomBoss::collideWithAvatar(Context & context, const sf::FloatRect & avatarRect)
    {
        // this function only detects the player entering the fight region for the first time
        if (!m_isThereABossOnThisLevel)
        {
            return;
        }

        if (m_hasFightBegun)
        {
            return;
        }

        if (avatarRect.intersects(m_region))
        {
            m_hasFightBegun = true;
            context.audio.play("mushroom-enrage");
            m_state = BossState::Shake;
        }
    }

    AvatarAnim & MushroomBoss::currentAnim()
    {
        // clang-format off
        switch (m_state)
        {
            case BossState::Jump:   { return m_jumpAnim;   }
            case BossState::Attack: { return m_attackAnim; }
            case BossState::Hit:    { return m_hitAnim;    }
            case BossState::Shake:  { return m_shakeAnim;  }
            case BossState::Death:  { return m_deathAnim;  }
            case BossState::Advance:
            case BossState::Idle:
            default:                { return m_idleAnim; }
        }
        // clang-format on
    }

    const BossCollRects MushroomBoss::collisionRects() const
    {
        BossCollRects rects;

        const sf::FloatRect globalBounds{ m_sprite.getGlobalBounds() };

        rects.top = globalBounds;
        rects.top.left += (globalBounds.width * 0.2f);
        rects.top.width -= (globalBounds.width * 0.4f);
        rects.top.top += (globalBounds.height * 0.45f);
        rects.top.height *= 0.25f;
        rects.top.left *= 1.025f;

        rects.bottom = globalBounds;
        rects.bottom.left += (globalBounds.width * 0.333f);
        rects.bottom.width -= (globalBounds.width * 0.666f);
        rects.bottom.top = util::bottom(rects.top) + 2.0f;
        rects.bottom.height = (util::bottom(globalBounds) - rects.bottom.top);
        rects.bottom.left += (globalBounds.width * 0.08f);
        rects.bottom.width *= 0.8f;

        return rects;
    }

    void MushroomBoss::appendCollisions(std::vector<sf::FloatRect> & collRects) const
    {
        const auto rects = collisionRects();
        collRects.push_back(rects.top);
        collRects.push_back(rects.bottom);
    }

} // namespace halloween
