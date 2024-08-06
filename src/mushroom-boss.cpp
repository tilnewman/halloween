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
        , m_elapsedTimeSec(0.0f)
        , m_sprite()
        , m_region()
        , m_isThereABossOnThisLevel(false)
    {}

    void MushroomBoss::setup(const Settings & settings)
    {
        m_idleAnim.setup((settings.media_path / "image/mushroom"), "idle", 31, 0.05f, true);
        m_jumpAnim.setup((settings.media_path / "image/mushroom"), "jump", 31, 0.1f, true);
        m_attackAnim.setup((settings.media_path / "image/mushroom"), "attack", 18, 0.175f, true);
        m_hitAnim.setup((settings.media_path / "image/mushroom"), "hit", 6, 0.175f, true);
        m_shakeAnim.setup((settings.media_path / "image/mushroom"), "shake", 5, 0.25f, true);
        m_deathAnim.setup((settings.media_path / "image/mushroom"), "death", 23, 0.2f, true);

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

        if (m_idleAnim.update(frameTimeSec))
        {
            m_sprite.setTexture(m_idleAnim.texture(), true);
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

} // namespace halloween
