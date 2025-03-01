// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// coin.cpp
//
#include "coin.hpp"

#include "context.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "level.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"
#include "texture-loader.hpp"

#include <algorithm>

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{
    Coin::Coin(const sf::Texture & texture, const sf::Vector2f & position)
        : is_alive(true)
        , sprite(texture, { { 0, 0 }, { 64, 64 } })
        , anim_index(0)
    {
        sprite.setScale({ 0.5f, 0.75f });
        util::setOriginToCenter(sprite);
        sprite.setPosition(position);
    }

    //

    CoinAnim::CoinAnim(const sf::Texture & texture)
        : is_alive(true)
        , sprite(texture)
    {}

    //

    Coins::Coins()
        : m_texture()
        , m_textureCoords()
        , m_timePerFrame(0.075f)
        , m_coins()
        , m_elapsedTimeSec(0.0f)
        , m_textureIndex(0)
        , m_animations()
    {
        // anything more than dozens will work here
        m_coins.reserve(100);
        m_animations.reserve(100);

        // animation frames in order within the spritesheet
        m_textureCoords.emplace_back(sf::Vector2i{ 0, 0 }, sf::Vector2i{ 64, 64 });
        m_textureCoords.emplace_back(sf::Vector2i{ 64, 0 }, sf::Vector2i{ 64, 64 });
        m_textureCoords.emplace_back(sf::Vector2i{ 128, 0 }, sf::Vector2i{ 64, 64 });
        m_textureCoords.emplace_back(sf::Vector2i{ 192, 0 }, sf::Vector2i{ 64, 64 });
        m_textureCoords.emplace_back(sf::Vector2i{ 256, 0 }, sf::Vector2i{ 64, 64 });
        m_textureCoords.emplace_back(sf::Vector2i{ 0, 64 }, sf::Vector2i{ 64, 64 });
        m_textureCoords.emplace_back(sf::Vector2i{ 64, 64 }, sf::Vector2i{ 64, 64 });
        m_textureCoords.emplace_back(sf::Vector2i{ 128, 64 }, sf::Vector2i{ 64, 64 });
        m_textureCoords.emplace_back(sf::Vector2i{ 192, 64 }, sf::Vector2i{ 64, 64 });
        m_textureCoords.emplace_back(sf::Vector2i{ 256, 64 }, sf::Vector2i{ 64, 64 });
    }

    void Coins::setup(const Settings & settings)
    {
        util::TextureLoader::load(m_texture, (settings.media_path / "image/coin.png"), true);
    }

    void Coins::add(Context &, const sf::FloatRect & region)
    {
        m_coins.emplace_back(m_texture, util::center(region));
    }

    void Coins::clear() { m_coins.clear(); }

    void Coins::update(Context &, const float frameTimeSec)
    {
        updateTextures(frameTimeSec);
        updateAnimations(frameTimeSec);
    }

    void Coins::updateAnimations(const float frameTimeSec)
    {
        bool wereAnyKilled = false;
        for (CoinAnim & anim : m_animations)
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
            }

            const float scaleSpeed{ 2.0f };
            const float newScale{ 1.0f + (frameTimeSec * scaleSpeed) };
            anim.sprite.scale({ newScale, newScale });
        }

        if (wereAnyKilled)
        {
            m_animations.erase(
                std::remove_if(
                    std::begin(m_animations),
                    std::end(m_animations),
                    [](const CoinAnim & anim) { return !anim.is_alive; }),
                std::end(m_animations));
        }
    }

    void Coins::updateTextures(const float frameTimeSec)
    {
        // all coins spin at the same time and rate
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

        for (Coin & coin : m_coins)
        {
            coin.sprite.setTextureRect(m_textureCoords.at(m_textureIndex));
        }
    }

    void Coins::draw(
        const Context & context, sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (const Coin & coin : m_coins)
        {
            if (context.layout.mapRegion().findIntersection(coin.sprite.getGlobalBounds()))
            {
                target.draw(coin.sprite, states);
            }
        }

        for (const CoinAnim & coinAnim : m_animations)
        {
            if (context.layout.mapRegion().findIntersection(coinAnim.sprite.getGlobalBounds()))
            {
                target.draw(coinAnim.sprite, states);
            }
        }
    }

    void Coins::moveWithMap(const sf::Vector2f & move)
    {
        for (Coin & coin : m_coins)
        {
            coin.sprite.move(move);
        }

        for (CoinAnim & anim : m_animations)
        {
            anim.sprite.move(move);
        }
    }

    void Coins::collideWithAvatar(Context & context, const sf::FloatRect & avatarRect)
    {
        bool wereAnyCollected = false;
        for (Coin & coin : m_coins)
        {
            const sf::FloatRect coinRect = coin.sprite.getGlobalBounds();

            if (avatarRect.findIntersection(coinRect))
            {
                wereAnyCollected = true;
                coin.is_alive = false;
                context.audio.play("coin");
                context.info_region.scoreAdjust(1);
                ++context.stats.coin_collected;
                addAnimation(context, util::center(coinRect));
            }
        }

        if (wereAnyCollected)
        {
            m_coins.erase(
                std::remove_if(
                    std::begin(m_coins),
                    std::end(m_coins),
                    [](const Coin & coin) { return !coin.is_alive; }),
                std::end(m_coins));
        }
    }

    void Coins::addAnimation(const Context & context, const sf::Vector2f & position)
    {
        if (!context.settings.will_show_coin_animations)
        {
            return;
        }

        CoinAnim & anim = m_animations.emplace_back(m_texture);
        anim.sprite.setTextureRect(m_textureCoords.at(0));
        util::setOriginToCenter(anim.sprite);
        anim.sprite.setPosition(position);
    }

} // namespace halloween
