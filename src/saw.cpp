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
        m_saws.reserve(100);
    }

    void Saws::setup(const Settings & settings)
    {
        m_texture.loadFromFile((settings.media_path / "image" / "saw.png").string());
        m_texture.setSmooth(true);
    }

    void Saws::add(const Context &, const sf::FloatRect & region)
    {
        Saw & saw = m_saws.emplace_back();

        saw.sprite.setTexture(m_texture);
        saw.sprite.setScale(m_scale);
        util::setOriginToCenter(saw.sprite);
        saw.sprite.setPosition(util::center(region));
    }

    void Saws::clear() { m_saws.clear(); }

    void Saws::update(const float frameTimeSec)
    {
        for (Saw & saw : m_saws)
        {
            saw.sprite.rotate(m_rotationSpeed * frameTimeSec);
        }
    }

    void Saws::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (const Saw & saw : m_saws)
        {
            target.draw(saw.sprite, states);
        }
    }

    void Saws::move(const sf::Vector2f & move)
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

            if (distance < (sawBounds.width * 0.5f))
            {
                return true;
            }
        }

        return false;
    }

} // namespace halloween
