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
        , m_hitPoints(0)
        , m_hitPointsMax(0)
    {}

    void MushroomBoss::setup(const Settings & settings)
    {
        m_idleAnim.setup((settings.media_path / "image/mushroom"), "idle", 31, 0.05f, true);
        m_jumpAnim.setup((settings.media_path / "image/mushroom"), "jump", 31, 0.05f, false);
        m_attackAnim.setup((settings.media_path / "image/mushroom"), "attack", 18, 0.05f, false);
        m_hitAnim.setup((settings.media_path / "image/mushroom"), "hit", 6, 0.05f, false);
        m_shakeAnim.setup((settings.media_path / "image/mushroom"), "shake", 5, 0.15f, false);
        m_deathAnim.setup((settings.media_path / "image/mushroom"), "death", 23, 0.05f, false);

        m_sprite.setTexture(m_idleAnim.texture(), true);
        m_sprite.scale(1.0f, 1.0f);

        m_hitPointsMax = settings.boss_hit_points;
    }

    void MushroomBoss::add(Context &, const sf::FloatRect & rect)
    {
        m_region = rect;
        m_isThereABossOnThisLevel = true;
        m_hitPoints = m_hitPointsMax;

        m_sprite.setPosition(
            (util::right(m_region) - m_sprite.getGlobalBounds().width),
            (util::bottom(m_region) - m_sprite.getGlobalBounds().height));
    }

    void MushroomBoss::clear()
    {
        // reset everything so the boss can be fought again until the player kills it
        m_state = BossState::Idle;
        m_idleAnim.restart();
        m_jumpAnim.restart();
        m_attackAnim.restart();
        m_hitAnim.restart();
        m_shakeAnim.restart();
        m_deathAnim.restart();
        m_isThereABossOnThisLevel = false;
        m_hasFightBegun = false;
        m_hitPoints = m_hitPointsMax;
    }

    void MushroomBoss::update(Context & context, const float frameTimeSec)
    {
        if (!m_isThereABossOnThisLevel)
        {
            return;
        }

        // progress animation
        AvatarAnim & anim{ currentAnim() };
        if (anim.update(frameTimeSec))
        {
            m_sprite.setTexture(anim.texture(), true);
        }

        if (anim.isFinished())
        {
            // change state
            if (BossState::Death != m_state)
            {
                if (0 == m_hitPoints)
                {
                    m_state = BossState::Death;
                    context.audio.play("mushroom-die");
                    context.info_region.scoreAdjust(context.settings.kill_boss_score);
                }
                else
                {
                    m_state = BossState::Advance;
                    currentAnim().restart();
                }
            }
        }

        // move
        if (BossState::Advance == m_state)
        {
            m_sprite.move((-20.0f * frameTimeSec), 0.0f);
            keepInRegion();
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

            // const auto rects = collisionRects();
            // util::drawRectangleShape(target, rects.top, false, sf::Color::Red);
            // util::drawRectangleShape(target, rects.middle, false, sf::Color::Red);
            // util::drawRectangleShape(target, rects.bottom, false, sf::Color::Red);
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
        if (!m_isThereABossOnThisLevel || m_hasFightBegun)
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

        rects.middle = globalBounds;
        rects.middle.left += (globalBounds.width * 0.2f);
        rects.middle.width -= (globalBounds.width * 0.4f);
        rects.middle.top += (globalBounds.height * 0.475f);
        rects.middle.height *= 0.25f;
        rects.middle.left *= 1.025f;

        rects.top = rects.middle;
        rects.top.height = (rects.middle.height * 0.35f);
        rects.top.top = (rects.middle.top - rects.top.height);
        rects.top.width *= 0.75f;
        rects.top.left = (util::center(rects.middle).x - (rects.top.width * 0.5f));

        rects.bottom = globalBounds;
        rects.bottom.left += (globalBounds.width * 0.333f);
        rects.bottom.width -= (globalBounds.width * 0.666f);
        rects.bottom.top = util::bottom(rects.middle);
        rects.bottom.height = (util::bottom(globalBounds) - rects.bottom.top);
        rects.bottom.left += (globalBounds.width * 0.08f);
        rects.bottom.width *= 0.8f;

        if (BossState::Jump == m_state)
        {
            float heightAdjRatio{ 0.0f };

            // clang-format off
            switch (m_jumpAnim.index())
            {
                case 6:  { heightAdjRatio = 0.16f; break; }
                case 7:  { heightAdjRatio = 0.24f; break; }
                case 8:  { heightAdjRatio = 0.28f; break; }
                case 9:  { heightAdjRatio = 0.31f; break; }
                case 10: { heightAdjRatio = 0.33f; break; }
                case 11: { heightAdjRatio = 0.35f; break; }
                case 12: { heightAdjRatio = 0.37f; break; }
                case 13: { heightAdjRatio = 0.39f; break; }
                case 14: { heightAdjRatio = 0.41f; break; }
                case 15: { heightAdjRatio = 0.35f; break; }
                case 16: { heightAdjRatio = 0.27f; break; }
                case 17: { heightAdjRatio = 0.20f; break; }
                case 18: { heightAdjRatio = 0.14f; break; }
                case 19: { heightAdjRatio = 0.08f; break; }
                default: { heightAdjRatio = 0.00f; break; }
            }
            // clang-format on

            const float heightAdj{ (heightAdjRatio * globalBounds.height) };
            rects.top.top -= heightAdj;
            rects.middle.top -= heightAdj;
            rects.bottom.top -= heightAdj;
        }

        return rects;
    }

    bool MushroomBoss::doesCollide(const sf::FloatRect & rect) const
    {
        if (!m_isThereABossOnThisLevel || (BossState::Death == m_state))
        {
            return false;
        }

        const BossCollRects bossRects{ collisionRects() };

        return (
            rect.intersects(bossRects.top) || rect.intersects(bossRects.middle) ||
            rect.intersects(bossRects.bottom));
    }

    bool MushroomBoss::attack(Context & context, const sf::FloatRect & attackRect)
    {
        if (!doesCollide(attackRect))
        {
            return false;
        }

        --m_hitPoints;
        m_state = BossState::Hit;
        context.audio.play("mushroom-hit");
        currentAnim().restart();

        m_sprite.move(15.0f, 0.0f);
        keepInRegion();

        return true;
    }

    void MushroomBoss::keepInRegion()
    {
        const sf::FloatRect bounds{ m_sprite.getGlobalBounds() };

        if (bounds.left < m_region.left)
        {
            m_sprite.move((m_region.left - bounds.left), 0.0f);
        }
        else if (util::right(bounds) > util::right(m_region))
        {
            m_sprite.move(-(util::right(bounds) - util::right(m_region)), 0.0f);
        }
    }

} // namespace halloween
