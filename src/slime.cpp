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
        , m_timePerTextureSec(0.1f)
    {
        // probably never more than one dozen of each in a level
        m_greenSpawnRects.reserve(100);
        m_orangeSpawnRects.reserve(100);
        m_slimes.reserve(100);
    }

    void Slimes::setup(const Settings & settings)
    {
        const std::size_t textureCount = 30;

        m_greenTextures.resize(textureCount);
        for (std::size_t i(0); i < textureCount; ++i)
        {
            std::string str;
            str = (settings.media_path / "image/slime-green" / "slime-green-").string();
            str += std::to_string(i);
            str += ".png";

            m_greenTextures.at(i).loadFromFile(str);
            m_greenTextures.at(i).setSmooth(true);
        }

        m_orangeTextures.resize(textureCount);
        for (std::size_t i(0); i < textureCount; ++i)
        {
            std::string str;
            str = (settings.media_path / "image/slime-orange" / "slime-orange-").string();
            str += std::to_string(i);
            str += ".png";

            m_orangeTextures.at(i).loadFromFile(str);
            m_orangeTextures.at(i).setSmooth(true);
        }
    }

    void Slimes::update(const Context &, const float)
    {
        // TODO

        m_slimes.erase(
            std::remove_if(
                std::begin(m_slimes),
                std::end(m_slimes),
                [](const Slime & slime) { return !slime.is_alive; }),
            std::end(m_slimes));
    }

    void Slimes::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        states.blendMode = sf::BlendAdd;

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
