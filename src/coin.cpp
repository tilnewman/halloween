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
#include "sound-player.hpp"
#include "util.hpp"

#include <algorithm>

namespace slnghn
{
    Coin::Coin(const sf::Texture & TEXTURE, const sf::Vector2f & POSITION)
        : is_alive(true)
        , sprite(TEXTURE, { 0, 0, 64, 64 })
        , anim_index(0)
    {
        sprite.setScale(0.5f, 0.75f);
        util::setOriginToCenter(sprite);
        sprite.setPosition(POSITION);
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
        // anything more than "dozens" will work here
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
        const std::string PATH = (settings.media_path / "image" / "coin.png").string();
        m_texture.loadFromFile(PATH);
        m_texture.setSmooth(true);
    }

    void Coins::add(const sf::Vector2f & POSITION) { m_coins.emplace_back(m_texture, POSITION); }

    void Coins::clear() { m_coins.clear(); }

    void Coins::update(Context &, const float FRAME_TIME_SEC)
    {
        m_elapsedTimeSec += FRAME_TIME_SEC;
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

        for (Coin & coin : m_coins)
        {
            coin.sprite.setTextureRect(m_textureCoords.at(m_textureIndex));
        }
    }

    void Coins::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (const Coin & COIN : m_coins)
        {
            if (!COIN.is_alive)
            {
                continue;
            }

            target.draw(COIN.sprite, states);
        }
    }

    void Coins::move(const sf::Vector2f & MOVE)
    {
        for (Coin & coin : m_coins)
        {
            coin.sprite.move(MOVE);
        }
    }

    void Coins::collideWithAvatar(Context & context, const sf::FloatRect & AVATAR_RECT)
    {
        for (Coin & coin : m_coins)
        {
            if (AVATAR_RECT.intersects(coin.sprite.getGlobalBounds()))
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
                [](const Coin & COIN) { return !COIN.is_alive; }),
            std::end(m_coins));
    }

} // namespace slnghn
