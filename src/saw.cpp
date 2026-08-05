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
        : m_texture{}
        , m_scale{ 0.5f, 0.5f }
        , m_rotationSpeed{ -200.0f }
        , m_saws{}
    {
        // probably no more than a dozen in any given map
        m_saws.reserve(16);
    }

    void Saws::setup(const Context & t_context)
    {
        util::TextureLoader::load(m_texture, (t_context.settings.media_path / "image" / "saw.png"));
    }

    void Saws::add(const Context &, const sf::FloatRect & t_region, const std::string &)
    {
        Saw & saw{ m_saws.emplace_back(m_texture) };
        saw.sprite.setScale(m_scale);
        util::setOriginToCenter(saw.sprite);
        saw.sprite.setPosition(util::center(t_region));
    }

    void Saws::clear() { m_saws.clear(); }

    void Saws::update(const Context &, const float t_frameTimeSec)
    {
        for (Saw & saw : m_saws)
        {
            saw.sprite.rotate(sf::degrees(m_rotationSpeed * t_frameTimeSec));
        }
    }

    void Saws::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        const sf::FloatRect wholeRect{ t_context.layout.wholeRegion() };

        for (const Saw & saw : m_saws)
        {
            if (wholeRect.findIntersection(saw.sprite.getGlobalBounds()))
            {
                t_target.draw(saw.sprite, t_states);
            }
        }
    }

    void Saws::moveWithMap(const sf::Vector2f & t_move)
    {
        for (Saw & saw : m_saws)
        {
            saw.sprite.move(t_move);
        }
    }

    bool Saws::doesAvatarCollideWithAnyAndDie(const Context &, const sf::FloatRect & t_avatarRect)
    {
        const sf::Vector2f avatarCenterPos = util::center(t_avatarRect);

        for (const Saw & saw : m_saws)
        {
            const sf::FloatRect sawBounds{ saw.sprite.getGlobalBounds() };
            const sf::Vector2f sawCenterPos{ util::center(sawBounds) };
            const float distance{ util::distance(avatarCenterPos, sawCenterPos) };
            if (distance < (sawBounds.size.x * 0.5f))
            {
                return true;
            }
        }

        return false;
    }

} // namespace halloween
