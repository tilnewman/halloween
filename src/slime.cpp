// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// slime.cpp
//
#include "slime.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "info-region.hpp"
#include "level.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"

#include <algorithm>
#include <iostream>

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    Slimes::Slimes()
        : m_greenTextures()
        , m_orangeTextures()
        , m_greenSpawnRects()
        , m_orangeSpawnRects()
        , m_slimes()
        , m_timePerTextureSec(0.0333f)
        , m_elapsedTimeSec(0.0f)
        , m_textureCount(30)
    {
        // probably never more than one dozen of each in a level
        m_greenSpawnRects.reserve(100);
        m_orangeSpawnRects.reserve(100);
        m_slimes.reserve(100);
    }

    void Slimes::setup(const Settings & settings)
    {
        m_greenTextures.resize(m_textureCount);
        for (std::size_t i(0); i < m_textureCount; ++i)
        {
            std::string str;
            str = (settings.media_path / "image/slime-green" / "slime-green-").string();
            str += std::to_string(i);
            str += ".png";

            m_greenTextures.at(i).loadFromFile(str);
            m_greenTextures.at(i).setSmooth(true);
        }

        m_orangeTextures.resize(m_textureCount);
        for (std::size_t i(0); i < m_textureCount; ++i)
        {
            std::string str;
            str = (settings.media_path / "image/slime-orange" / "slime-orange-").string();
            str += std::to_string(i);
            str += ".png";

            m_orangeTextures.at(i).loadFromFile(str);
            m_orangeTextures.at(i).setSmooth(true);
        }
    }

    void Slimes::addAll(const Context & context)
    {
        const float speed{ 50.0f };

        for (const sf::FloatRect & rect : m_greenSpawnRects)
        {
            Slime slime(true, context.random.boolean(), rect, speed);

            slime.texture_index = context.random.index(m_greenTextures);
            slime.sprite.setTexture(m_greenTextures.at(slime.texture_index));
            slime.sprite.setScale({ 0.35f, 0.35f });

            const float posX{ rect.left + (rect.width / 2.0f) };
            const float posY{ (rect.top + rect.height) - slime.sprite.getGlobalBounds().height };
            slime.sprite.setPosition(posX, posY);

            m_slimes.push_back(slime);
        }

        for (const sf::FloatRect & rect : m_orangeSpawnRects)
        {
            Slime slime(false, context.random.boolean(), rect, speed);

            slime.texture_index = context.random.index(m_orangeTextures);
            slime.sprite.setTexture(m_orangeTextures.at(slime.texture_index));
            slime.sprite.setScale({ 0.5f, 0.5f });

            const float posX{ rect.left + (rect.width / 2.0f) };
            const float posY{ (rect.top + rect.height) - slime.sprite.getGlobalBounds().height };
            slime.sprite.setPosition(posX, posY);

            m_slimes.push_back(slime);
        }
    }

    void Slimes::update(const Context &, const float frameTimeSec)
    {
        m_elapsedTimeSec += frameTimeSec;
        if (m_elapsedTimeSec > m_timePerTextureSec)
        {
            for (Slime & slime : m_slimes)
            {
                ++slime.texture_index;
                if (slime.texture_index >= m_textureCount)
                {
                    slime.texture_index = 0;
                }

                if (slime.is_green)
                {
                    slime.sprite.setTexture(m_greenTextures.at(slime.texture_index));
                }
                else
                {
                    slime.sprite.setTexture(m_orangeTextures.at(slime.texture_index));
                }
            }

            m_elapsedTimeSec -= m_timePerTextureSec;
        }

        m_slimes.erase(
            std::remove_if(
                std::begin(m_slimes),
                std::end(m_slimes),
                [](const Slime & slime) { return !slime.is_alive; }),
            std::end(m_slimes));
    }

    void Slimes::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        // states.blendMode = sf::BlendAdd;

        for (const Slime & slime : m_slimes)
        {
            target.draw(slime.sprite, states);
        }
    }

    void Slimes::move(const sf::Vector2f & move)
    {
        for (sf::FloatRect & rect : m_greenSpawnRects)
        {
            rect.left += move.x;
        }

        for (sf::FloatRect & rect : m_orangeSpawnRects)
        {
            rect.left += move.x;
        }

        for (Slime & slime : m_slimes)
        {
            slime.sprite.move(move);
        }
    }

} // namespace halloween
