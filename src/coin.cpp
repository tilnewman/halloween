// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// coin.cpp
//
#include "coin.hpp"

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
    Coin::Coin(const sf::Texture & texture, const sf::Vector2f & position)
        : is_alive(true)
        , sprite(texture, { 0, 0, 64, 64 })
        , anim_index(0)
    {
        sprite.setScale(0.5f, 0.75f);
        util::setOriginToCenter(sprite);
        sprite.setPosition(position);
    }

    //

    Coins::Coins()
        : m_texture()
        , m_textureCoords()
        , m_timePerFrame(0.075f)
        , m_coins()
        , m_elapsedTimeSec(0.0f)
        , m_textureIndex(0)
    {
        // anything more than dozens will work here
        m_coins.reserve(100);

        // animation frames in order within the spritesheet
        m_textureCoords.push_back({ 0, 0, 64, 64 });
        m_textureCoords.push_back({ 64, 0, 64, 64 });
        m_textureCoords.push_back({ 128, 0, 64, 64 });
        m_textureCoords.push_back({ 192, 0, 64, 64 });
        m_textureCoords.push_back({ 256, 0, 64, 64 });
        m_textureCoords.push_back({ 0, 64, 64, 64 });
        m_textureCoords.push_back({ 64, 64, 64, 64 });
        m_textureCoords.push_back({ 128, 64, 64, 64 });
        m_textureCoords.push_back({ 192, 64, 64, 64 });
        m_textureCoords.push_back({ 256, 64, 64, 64 });
    }

    void Coins::setup(const Settings & settings)
    {
        const std::string filePath = (settings.media_path / "image" / "coin.png").string();
        m_texture.loadFromFile(filePath);
        m_texture.setSmooth(true);
    }

    void Coins::add(const sf::Vector2f & position) { m_coins.emplace_back(m_texture, position); }

    void Coins::clear() { m_coins.clear(); }

    void Coins::update(Context &, const float frameTimeSec)
    {
        m_elapsedTimeSec += frameTimeSec;
        if (m_elapsedTimeSec < m_timePerFrame)
        {
            return;
        }

        m_elapsedTimeSec -= m_timePerFrame;

        ++m_textureIndex;
        if (m_textureIndex >= m_textureCoords.size())
        {
            m_textureIndex = 0;
        }

        // all coins spin at the same time and rate
        for (Coin & coin : m_coins)
        {
            coin.sprite.setTextureRect(m_textureCoords.at(m_textureIndex));
        }
    }

    void Coins::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (const Coin & coin : m_coins)
        {
            if (!coin.is_alive)
            {
                continue;
            }

            target.draw(coin.sprite, states);
        }
    }

    void Coins::move(const sf::Vector2f & move)
    {
        for (Coin & coin : m_coins)
        {
            coin.sprite.move(move);
        }
    }

    void Coins::collideWithAvatar(Context & context, const sf::FloatRect & avatarRect)
    {
        for (Coin & coin : m_coins)
        {
            if (avatarRect.intersects(coin.sprite.getGlobalBounds()))
            {
                coin.is_alive = false;
                context.audio.play("coin");
                context.info_region.scoreAdjust(1);
            }
        }

        m_coins.erase(
            std::remove_if(
                std::begin(m_coins),
                std::end(m_coins),
                [](const Coin & coin) { return !coin.is_alive; }),
            std::end(m_coins));
    }

} // namespace halloween
