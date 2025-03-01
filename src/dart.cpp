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
        : m_texture()
        , m_darts()
        , m_dartAnims()
        , m_scale(0.6f, 0.8f)
    {
        // anything more than dozens will work here
        m_darts.reserve(32);
        m_dartAnims.reserve(32);
    }

    void Darts::setup(const Settings & settings)
    {
        util::TextureLoader::load(m_texture, (settings.media_path / "image/kunai.png"), true);
    }

    void Darts::add(Context &, const sf::FloatRect & region)
    {
        Dart & dart = m_darts.emplace_back(m_texture);
        dart.sprite.setScale(m_scale);
        util::setOriginToCenter(dart.sprite);
        dart.sprite.setPosition(util::center(region));
    }

    void Darts::clear() { m_darts.clear(); }

    void Darts::update(Context &, const float frameTimeSec)
    {
        bool wereAnyKilled = false;
        for (DartAnim & anim : m_dartAnims)
        {
            std::uint8_t alpha = anim.sprite.getColor().a;
            if (alpha >= 10)
            {
                alpha -= 10;
                anim.sprite.setColor(sf::Color(255, 255, 255, alpha));
            }
            else
            {
                anim.is_alive = false;
                wereAnyKilled = true;
                continue;
            }

            const float scaleSpeed{ 2.0f };
            const float newScale{ 1.0f + (frameTimeSec * scaleSpeed) };
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
        const Context & context, sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (const Dart & dart : m_darts)
        {
            if (context.layout.mapRegion().findIntersection(dart.sprite.getGlobalBounds()))
            {
                target.draw(dart.sprite, states);
            }
        }

        for (const DartAnim & dartAnim : m_dartAnims)
        {
            target.draw(dartAnim.sprite, states);
        }
    }

    void Darts::moveWithMap(const sf::Vector2f & move)
    {
        for (Dart & dart : m_darts)
        {
            dart.sprite.move(move);
        }

        for (DartAnim & anim : m_dartAnims)
        {
            anim.sprite.move(move);
        }
    }

    void Darts::collideWithAvatar(Context & context, const sf::FloatRect & avatarRect)
    {
        bool wereAnyCollected = false;
        for (Dart & dart : m_darts)
        {
            const sf::FloatRect dartRect = dart.sprite.getGlobalBounds();

            if (avatarRect.findIntersection(dartRect))
            {
                wereAnyCollected = true;
                dart.is_alive = false;
                context.audio.play("unsheath");
                context.info_region.dartsAdjust(1);

                DartAnim & anim = m_dartAnims.emplace_back(m_texture);
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
