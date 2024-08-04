// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// fire-spout.cpp
//
#include "fire-spout.hpp"

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

    FireSpouts::FireSpouts()
        : m_spoutTexture()
        , m_timePerFrame(0.1f)
        , m_timeBetweenSpurtsSec(2.5f)
        , m_elapsedTimeSec(0.0f)
        , m_fireSpouts()
        , m_fireTextures()
        , m_fireTextureIndex(0)
        , is_spurting(true)
    {
        // probably no more than a dozen in any given map
        m_fireSpouts.reserve(100);
    }

    void FireSpouts::setup(const Settings & settings)
    {
        m_spoutTexture.loadFromFile((settings.media_path / "image" / "fire-spouts.png").string());
        m_spoutTexture.setSmooth(true);

        const std::size_t fireTextureCount{ 14 };
        m_fireTextures.reserve(fireTextureCount);
        for (std::size_t i(1); i <= fireTextureCount; ++i)
        {
            std::string filePath = (settings.media_path / "image/fire/" / "fire-").string();
            filePath += std::to_string(i);
            filePath += ".png";

            sf::Texture & texture = m_fireTextures.emplace_back();
            texture.loadFromFile(filePath);
            texture.setSmooth(true);
        }
    }

    void FireSpouts::add(const Context &, const sf::FloatRect & region)
    {
        FireSpout & spout = m_fireSpouts.emplace_back();

        spout.spout_sprite.setTexture(m_spoutTexture);
        spout.spout_sprite.setTextureRect({ 0, 10, 35, 22 });
        spout.spout_sprite.setScale(2.0f, 2.0f);

        const sf::FloatRect spoutBounds = spout.spout_sprite.getGlobalBounds();

        spout.spout_sprite.setPosition(
            (util::center(region).x - (spoutBounds.width * 0.5f)),
            (util::bottom(region) - spoutBounds.height));

        spout.fire_sprite.setTexture(m_fireTextures.at(0));
        spout.fire_sprite.setScale(2.0f, 2.0f);

        const sf::FloatRect fireBounds = spout.fire_sprite.getGlobalBounds();

        spout.fire_sprite.setPosition(
            (util::center(spoutBounds).x - (fireBounds.width * 0.5f)),
            (spout.spout_sprite.getPosition().y - fireBounds.height));
    }

    void FireSpouts::clear() { m_fireSpouts.clear(); }

    void FireSpouts::update(const float frameTimeSec)
    {
        // all spouts burst fire at the same time and rate
        m_elapsedTimeSec += frameTimeSec;
        if (m_elapsedTimeSec < m_timePerFrame)
        {
            return;
        }

        m_elapsedTimeSec -= m_timePerFrame;

        ++m_fireTextureIndex;
        if (m_fireTextureIndex >= m_fireTextures.size())
        {
            m_fireTextureIndex = 0;
        }

        for (FireSpout & spout : m_fireSpouts)
        {
            spout.fire_sprite.setTexture(m_fireTextures.at(m_fireTextureIndex), true);

            const sf::FloatRect fireBounds = spout.fire_sprite.getGlobalBounds();

            spout.fire_sprite.setPosition(
                (util::center(spout.spout_sprite.getGlobalBounds()).x - (fireBounds.width * 0.5f)),
                (spout.spout_sprite.getPosition().y - fireBounds.height));
        }
    }

    void FireSpouts::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (const FireSpout & spout : m_fireSpouts)
        {
            target.draw(spout.spout_sprite, states);
            target.draw(spout.fire_sprite, states);
        }
    }

    void FireSpouts::move(const sf::Vector2f & move)
    {
        for (FireSpout & spout : m_fireSpouts)
        {
            spout.spout_sprite.move(move);
            spout.fire_sprite.move(move);
        }
    }

    bool FireSpouts::doesCollideWithAny(const sf::FloatRect & avatarRect) const
    {
        if (!is_spurting)
        {
            return false;
        }

        for (const FireSpout & spout : m_fireSpouts)
        {
            const sf::FloatRect spoutRect = spout.fire_sprite.getGlobalBounds();

            if (avatarRect.intersects(spoutRect))
            {
                return true;
            }
        }

        return false;
    }

    const std::vector<sf::FloatRect> FireSpouts::collisions() const
    {
        std::vector<sf::FloatRect> rects;
        rects.reserve(m_fireSpouts.size());

        for (const FireSpout & spout : m_fireSpouts)
        {
            rects.push_back(spout.spout_sprite.getGlobalBounds());
        }

        return rects;
    }

} // namespace halloween
