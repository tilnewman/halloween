// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// blood.cpp
//
#include "blood.hpp"

#include "context.hpp"
#include "info-region.hpp"
#include "level.hpp"
#include "random.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "util.hpp"

namespace halloween
{
    Blood::Blood()
        : m_texture()
        , m_textureCoords1()
        , m_textureCoords2()
        , m_isUsingFirstAnim(true)
        , m_timePerFrame(0.05f)
        , m_elapsedTimeSec(0.0f)
        , m_textureIndex(0)
        , m_sprite()
        , m_isFinished(true)
    {
        // there are two blood splat animation in the same texture
        m_textureCoords1.emplace_back(0, 0, 128, 128);
        m_textureCoords1.emplace_back(128, 0, 128, 128);
        m_textureCoords1.emplace_back(256, 0, 128, 128);
        m_textureCoords1.emplace_back(384, 0, 128, 128);
        m_textureCoords1.emplace_back(512, 0, 128, 128);
        m_textureCoords1.emplace_back(640, 0, 128, 128);
        m_textureCoords1.emplace_back(768, 0, 128, 128);
        m_textureCoords1.emplace_back(896, 0, 128, 128);
        m_textureCoords1.emplace_back(1024, 0, 128, 128);

        m_textureCoords2.emplace_back(0, 128, 128, 128);
        m_textureCoords2.emplace_back(128, 128, 128, 128);
        m_textureCoords2.emplace_back(256, 128, 128, 128);
        m_textureCoords2.emplace_back(384, 128, 128, 128);
        m_textureCoords2.emplace_back(512, 128, 128, 128);
        m_textureCoords2.emplace_back(640, 128, 128, 128);
        m_textureCoords2.emplace_back(768, 128, 128, 128);
        m_textureCoords2.emplace_back(896, 128, 128, 128);
        m_textureCoords2.emplace_back(1024, 128, 128, 128);
    }

    void Blood::setup(const Settings & settings)
    {
        const std::string filePath = (settings.media_path / "image" / "blood.png").string();

        m_texture.loadFromFile(filePath);
        m_texture.setSmooth(true);

        m_sprite.setTexture(m_texture);
    }

    void Blood::start(Context & context, const sf::Vector2f & position, const bool willSplashRight)
    {
        m_isFinished = false;
        m_elapsedTimeSec = 0.0f;
        m_textureIndex = 0;
        m_isUsingFirstAnim = context.random.boolean();
        m_sprite.setPosition(position);

        if (m_isUsingFirstAnim)
        {
            m_sprite.setTextureRect(m_textureCoords1.at(0));
        }
        else
        {
            m_sprite.setTextureRect(m_textureCoords2.at(0));
        }

        if (willSplashRight)
        {
            m_sprite.setScale(1.0f, 1.0f);
        }
        else
        {
            m_sprite.setScale(-1.0f, 1.0f);
        }
    }

    void Blood::update(Context &, const float frameTimeSec)
    {
        if (m_isFinished)
        {
            return;
        }

        m_elapsedTimeSec += frameTimeSec;
        if (m_elapsedTimeSec < m_timePerFrame)
        {
            return;
        }

        m_elapsedTimeSec -= m_timePerFrame;

        ++m_textureIndex;
        if (m_textureIndex >= m_textureCoords1.size())
        {
            m_textureIndex = 0;
            m_isFinished = true;
            return;
        }

        if (m_isUsingFirstAnim)
        {
            m_sprite.setTextureRect(m_textureCoords1.at(m_textureIndex));
        }
        else
        {
            m_sprite.setTextureRect(m_textureCoords2.at(m_textureIndex));
        }
    }

    void Blood::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (!m_isFinished)
        {
            target.draw(m_sprite, states);
        }
    }

} // namespace halloween
