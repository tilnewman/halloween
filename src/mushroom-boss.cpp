// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// mushroom-boss.cpp
//
#include "mushroom-boss.hpp"

#include "avatar.hpp"
#include "check-macros.hpp"
#include "context.hpp"
#include "info-region.hpp"
#include "level.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"

#include <algorithm>

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    MushroomBoss::MushroomBoss()
        : m_state{ BossState::Idle }
        , m_idleAnim{}
        , m_jumpAnim{}
        , m_attackAnim{}
        , m_hitAnim{}
        , m_shakeAnim{}
        , m_deathAnim{}
        , m_sprite{ util::SfmlDefaults::instance().texture() }
        , m_region{}
        , m_isThereABossOnThisLevel{ false }
        , m_hasFightBegun{ false }
        , m_hitPoints{ 0 }
        , m_hitPointsMax{ 0 }
    {}

    void MushroomBoss::setup(const Context & t_context)
    {
        m_idleAnim.setup(
            (t_context.settings.media_path / "image" / "mushroom"), "idle", 31, 0.05f, true);

        m_jumpAnim.setup(
            (t_context.settings.media_path / "image" / "mushroom"), "jump", 31, 0.05f, false);

        m_attackAnim.setup(
            (t_context.settings.media_path / "image" / "mushroom"), "attack", 18, 0.05f, false);

        m_hitAnim.setup(
            (t_context.settings.media_path / "image" / "mushroom"), "hit", 6, 0.05f, false);

        m_shakeAnim.setup(
            (t_context.settings.media_path / "image" / "mushroom"), "shake", 5, 0.15f, false);

        m_deathAnim.setup(
            (t_context.settings.media_path / "image" / "mushroom"), "death", 23, 0.05f, false);

        m_sprite.setTexture(m_idleAnim.texture(), true);

        m_hitPointsMax = t_context.settings.boss_hit_points;
    }

    void MushroomBoss::add(const Context &, const sf::FloatRect & t_rect)
    {
        m_region = t_rect;
        m_isThereABossOnThisLevel = true;
        m_hitPoints = m_hitPointsMax;

        m_sprite.setPosition(
            { (util::right(m_region) - m_sprite.getGlobalBounds().size.x),
              (util::bottom(m_region) - m_sprite.getGlobalBounds().size.y) });
    }

    void MushroomBoss::clear()
    {
        // reset everything so the boss can be fought again
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

    void MushroomBoss::update(const Context & t_context, const float t_frameTimeSec)
    {
        if (!m_isThereABossOnThisLevel)
        {
            return;
        }

        // progress animation
        AvatarAnim & anim{ currentAnim() };
        if (anim.update(t_frameTimeSec))
        {
            m_sprite.setTexture(anim.texture(), true);

            if ((BossState::Jump == m_state) && (m_jumpAnim.index() == 19))
            {
                t_context.audio.play("slam", 0.75f);
            }
            else if (((BossState::Attack == m_state) && (m_attackAnim.index() >= 10)))
            {
                if (isPlayerWithinAttackRange(t_context, true))
                {
                    t_context.avatar.handleHitByBoss(t_context);
                }
            }
        }

        if (anim.isFinished() || ((BossState::Advance == m_state) && (m_idleAnim.index() == 0)))
        {
            // change state
            if (BossState::Death != m_state)
            {
                if (0 == m_hitPoints)
                {
                    setState(BossState::Death);
                    t_context.audio.play("mushroom-die");
                    t_context.info_region.scoreAdjust(t_context.settings.kill_boss_score);
                }
                else if (isPlayerWithinAttackRange(t_context, false) && !t_context.avatar.isDead())
                {
                    setState(BossState::Attack);
                    t_context.audio.play("mushroom-attack");
                }
                else if (BossState::Advance != m_state)
                {
                    setState(BossState::Advance);
                }
            }
        }

        // move
        if (BossState::Advance == m_state)
        {
            m_sprite.move(
                { (-1.0f * t_context.settings.mushroom_boss_walk_speed * t_frameTimeSec), 0.0f });

            keepInRegion();
        }
    }

    void MushroomBoss::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        if (!m_isThereABossOnThisLevel)
        {
            return;
        }

        if (t_context.layout.mapRegion().findIntersection(m_sprite.getGlobalBounds()))
        {
            t_target.draw(m_sprite, t_states);
        }
    }

    void MushroomBoss::moveWithMap(const sf::Vector2f & t_move)
    {
        if (!m_isThereABossOnThisLevel)
        {
            return;
        }

        m_sprite.move(t_move);
        m_region.position += t_move;
    }

    void MushroomBoss::collideWithAvatar(
        const Context & t_context, const sf::FloatRect & t_avatarRect)
    {
        // this function only detects the player entering the fight region for the first time
        if (!m_isThereABossOnThisLevel || m_hasFightBegun)
        {
            return;
        }

        if (t_avatarRect.findIntersection(m_region))
        {
            m_hasFightBegun = true;
            setState(BossState::Shake);
            t_context.audio.play("mushroom-enrage");
        }
    }

    void MushroomBoss::setState(const BossState newState)
    {
        m_state = newState;
        currentAnim().restart();
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
        rects.middle.position.x += (globalBounds.size.x * 0.2f);
        rects.middle.size.x -= (globalBounds.size.x * 0.4f);
        rects.middle.position.y += (globalBounds.size.y * 0.475f);
        rects.middle.size.y *= 0.25f;
        rects.middle.position.x *= 1.025f;

        rects.top = rects.middle;
        rects.top.size.y = (rects.middle.size.y * 0.35f);
        rects.top.position.y = (rects.middle.position.y - rects.top.size.y);
        rects.top.size.x *= 0.75f;
        rects.top.position.x = (util::center(rects.middle).x - (rects.top.size.x * 0.5f));

        rects.bottom = globalBounds;
        rects.bottom.position.x += (globalBounds.size.x * 0.333f);
        rects.bottom.size.x -= (globalBounds.size.x * 0.666f);
        rects.bottom.position.y = util::bottom(rects.middle);
        rects.bottom.size.y = (util::bottom(globalBounds) - rects.bottom.position.y);
        rects.bottom.position.x += (globalBounds.size.x * 0.08f);
        rects.bottom.size.x *= 0.8f;

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

            const float heightAdj{ (heightAdjRatio * globalBounds.size.y) };
            rects.top.position.y -= heightAdj;
            rects.middle.position.y -= heightAdj;
            rects.bottom.position.y -= heightAdj;
        }

        return rects;
    }

    bool MushroomBoss::doesCollide(const sf::FloatRect & rect) const
    {
        if (!m_isThereABossOnThisLevel || (BossState::Death == m_state) || !m_hasFightBegun)
        {
            return false;
        }

        const BossCollRects bossRects{ collisionRects() };

        return (
            rect.findIntersection(bossRects.top) || rect.findIntersection(bossRects.middle) ||
            rect.findIntersection(bossRects.bottom));
    }

    bool MushroomBoss::attack(const Context & context, const sf::FloatRect & attackRect)
    {
        if (!doesCollide(attackRect))
        {
            return false;
        }

        if (m_hitPoints > 0)
        {
            --m_hitPoints;
        }

        setState(BossState::Hit);
        context.audio.play("mushroom-hit");

        m_sprite.move({ 15.0f, 0.0f });
        keepInRegion();

        return true;
    }

    void MushroomBoss::keepInRegion()
    {
        const sf::FloatRect bounds{ m_sprite.getGlobalBounds() };

        if (bounds.position.x < m_region.position.x)
        {
            m_sprite.move({ (m_region.position.x - bounds.position.x), 0.0f });
        }
        else if (util::right(bounds) > util::right(m_region))
        {
            m_sprite.move({ -(util::right(bounds) - util::right(m_region)), 0.0f });
        }
    }

    void MushroomBoss::reactToThrow(const Context & t_context)
    {
        if (!m_isThereABossOnThisLevel || (BossState::Death == m_state) || !m_hasFightBegun)
        {
            return;
        }

        if ((BossState::Advance == m_state) || (BossState::Idle == m_state))
        {
            setState(BossState::Jump);
            t_context.audio.play("mushroom-jump");
        }
    }

    bool MushroomBoss::isPlayerWithinAttackRange(
        const Context & t_context, const bool t_isHitQuery) const
    {
        const float mutliplier{ (t_isHitQuery) ? 1.0f : 2.0f };
        const sf::FloatRect playerBounds{ t_context.avatar.bounds() };
        const float distance{ collisionRects().middle.position.x - util::right(playerBounds) };
        return (distance < (playerBounds.size.x * mutliplier));
    }

} // namespace halloween
