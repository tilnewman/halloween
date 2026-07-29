// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// dart.cpp
//
#include "dart.hpp"

#include "context.hpp"
#include "info-region.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"
#include "texture-loader.hpp"

#include <algorithm>

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    Darts::Darts()
        : m_texture{}
        , m_darts{}
        , m_dartAnims{}
        , m_scale{ 0.6f, 0.8f }
    {
        // anything more than a dozen will work here
        m_darts.reserve(32);
        m_dartAnims.reserve(32);
    }

    void Darts::setup(const Settings & t_settings)
    {
        util::TextureLoader::load(m_texture, (t_settings.media_path / "image" / "kunai.png"), true);
    }

    void Darts::add(const Context &, const sf::FloatRect & t_region)
    {
        Dart & dart{ m_darts.emplace_back(m_texture) };
        dart.sprite.setScale(m_scale);
        util::setOriginToCenter(dart.sprite);
        dart.sprite.setPosition(util::center(t_region));
    }

    void Darts::clear() { m_darts.clear(); }

    void Darts::update(const Context &, const float t_frameTimeSec)
    {
        bool wereAnyKilled{ false };
        for (DartAnim & anim : m_dartAnims)
        {
            std::uint8_t alpha = anim.sprite.getColor().a;
            if (alpha >= 10u)
            {
                alpha -= 10u;
                anim.sprite.setColor(sf::Color(255, 255, 255, alpha));
            }
            else
            {
                anim.is_alive = false;
                wereAnyKilled = true;
                continue;
            }

            const float scaleSpeed{ 2.0f };
            const float newScale{ 1.0f + (t_frameTimeSec * scaleSpeed) };
            anim.sprite.scale({ newScale, newScale });
        }

        if (wereAnyKilled)
        {
            m_dartAnims.erase(
                std::remove_if(
                    std::begin(m_dartAnims),
                    std::end(m_dartAnims),
                    [](const DartAnim & anim) { return !anim.is_alive; }),
                std::end(m_dartAnims));
        }
    }

    void Darts::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        const sf::FloatRect mapRect{ t_context.layout.mapRegion() };
        for (const Dart & dart : m_darts)
        {
            if (mapRect.findIntersection(dart.sprite.getGlobalBounds()))
            {
                t_target.draw(dart.sprite, t_states);
            }
        }

        for (const DartAnim & dartAnim : m_dartAnims)
        {
            t_target.draw(dartAnim.sprite, t_states);
        }
    }

    void Darts::moveWithMap(const sf::Vector2f & t_move)
    {
        for (Dart & dart : m_darts)
        {
            dart.sprite.move(t_move);
        }

        for (DartAnim & anim : m_dartAnims)
        {
            anim.sprite.move(t_move);
        }
    }

    void Darts::collideWithAvatar(const Context & t_context, const sf::FloatRect & t_avatarRect)
    {
        bool wereAnyCollected{ false };
        for (Dart & dart : m_darts)
        {
            const sf::FloatRect dartRect{ dart.sprite.getGlobalBounds() };

            if (t_avatarRect.findIntersection(dartRect))
            {
                wereAnyCollected = true;
                dart.is_alive = false;
                t_context.audio.play("unsheath");
                t_context.info_region.dartsAdjust(1);

                DartAnim & anim{ m_dartAnims.emplace_back(m_texture) };
                anim.sprite = dart.sprite;
            }
        }

        if (wereAnyCollected)
        {
            m_darts.erase(
                std::remove_if(
                    std::begin(m_darts),
                    std::end(m_darts),
                    [](const Dart & dart) { return !dart.is_alive; }),
                std::end(m_darts));
        }
    }

} // namespace halloween
