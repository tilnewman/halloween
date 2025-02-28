// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// saw.cpp
//
#include "saw.hpp"

#include "context.hpp"
#include "info-region.hpp"
#include "level.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"
#include "texture-loader.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    Saws::Saws()
        : m_texture()
        , m_scale(0.5f, 0.5f)
        , m_rotationSpeed(-200.0f)
        , m_saws()
    {
        // probably no more than a dozen in any given map
        m_saws.reserve(16);
    }

    void Saws::setup(const Settings & settings)
    {
        util::TextureLoader::load(m_texture, (settings.media_path / "image" / "saw.png"), true);
    }

    void Saws::add(Context &, const sf::FloatRect & region)
    {
        Saw & saw = m_saws.emplace_back(m_texture);
        saw.sprite.setScale(m_scale);
        util::setOriginToCenter(saw.sprite);
        saw.sprite.setPosition(util::center(region));
    }

    void Saws::clear() { m_saws.clear(); }

    void Saws::update(Context &, const float frameTimeSec)
    {
        for (Saw & saw : m_saws)
        {
            saw.sprite.rotate(sf::degrees(m_rotationSpeed * frameTimeSec));
        }
    }

    void Saws::draw(
        const Context & context, sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (const Saw & saw : m_saws)
        {
            if (context.layout.mapRegion().findIntersection(saw.sprite.getGlobalBounds()))
            {
                target.draw(saw.sprite, states);
            }
        }
    }

    void Saws::moveWithMap(const sf::Vector2f & move)
    {
        for (Saw & saw : m_saws)
        {
            saw.sprite.move(move);
        }
    }

    bool Saws::doesAvatarCollideWithAnyAndDie(const sf::FloatRect & avatarRect) const
    {
        const sf::Vector2f avatarCenterPos = util::center(avatarRect);

        for (const Saw & saw : m_saws)
        {
            const sf::FloatRect sawBounds = saw.sprite.getGlobalBounds();
            const sf::Vector2f sawCenterPos = util::center(sawBounds);
            const float distance = util::distance(avatarCenterPos, sawCenterPos);
            if (distance < (sawBounds.size.x * 0.5f))
            {
                return true;
            }
        }

        return false;
    }

} // namespace halloween
