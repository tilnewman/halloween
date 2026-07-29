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
    Coin::Coin(const sf::Texture & t_texture, const sf::Vector2f & t_position)
        : is_alive{ true }
        , sprite{ t_texture, { { 0, 0 }, { 64, 64 } } }
        , anim_index{ 0 }
    {
        sprite.setScale({ 0.5f, 0.75f });
        util::setOriginToCenter(sprite);
        sprite.setPosition(t_position);
    }

    //

    CoinAnim::CoinAnim(const sf::Texture & t_texture)
        : is_alive{ true }
        , sprite{ t_texture }
    {}

    //

    Coins::Coins()
        : m_texture{}
        , m_textureCoords{}
        , m_timePerFrame{ 0.075f }
        , m_coins{}
        , m_elapsedTimeSec{ 0.0f }
        , m_textureIndex{ 0 }
        , m_animations{}
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

    void Coins::setup(const Context & t_context)
    {
        util::TextureLoader::load(
            m_texture, (t_context.settings.media_path / "image" / "coin.png"), true);
    }

    void Coins::add(const Context &, const sf::FloatRect & t_region)
    {
        m_coins.emplace_back(m_texture, util::center(t_region));
    }

    void Coins::clear() { m_coins.clear(); }

    void Coins::update(const Context &, const float t_frameTimeSec)
    {
        updateTextures(t_frameTimeSec);
        updateAnimations(t_frameTimeSec);
    }

    void Coins::updateAnimations(const float t_frameTimeSec)
    {
        bool wereAnyKilled{ false };
        for (CoinAnim & anim : m_animations)
        {
            std::uint8_t alpha{ anim.sprite.getColor().a };
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
            const float newScale{ 1.0f + (t_frameTimeSec * scaleSpeed) };
            anim.sprite.scale({ newScale, newScale });
        }

        if (wereAnyKilled)
        {
            std::erase_if(m_animations, [](const CoinAnim & t_anim) { return !t_anim.is_alive; });
        }
    }

    void Coins::updateTextures(const float t_frameTimeSec)
    {
        // all coins spin at the same time and rate
        m_elapsedTimeSec += t_frameTimeSec;
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
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const Coin & coin : m_coins)
        {
            if (t_context.layout.mapRegion().findIntersection(coin.sprite.getGlobalBounds()))
            {
                t_target.draw(coin.sprite, t_states);
            }
        }

        for (const CoinAnim & coinAnim : m_animations)
        {
            if (t_context.layout.mapRegion().findIntersection(coinAnim.sprite.getGlobalBounds()))
            {
                t_target.draw(coinAnim.sprite, t_states);
            }
        }
    }

    void Coins::moveWithMap(const sf::Vector2f & t_move)
    {
        for (Coin & coin : m_coins)
        {
            coin.sprite.move(t_move);
        }

        for (CoinAnim & anim : m_animations)
        {
            anim.sprite.move(t_move);
        }
    }

    void Coins::collideWithAvatar(const Context & t_context, const sf::FloatRect & t_avatarRect)
    {
        bool wereAnyCollected{ false };
        for (Coin & coin : m_coins)
        {
            const sf::FloatRect coinRect = coin.sprite.getGlobalBounds();

            if (t_avatarRect.findIntersection(coinRect))
            {
                wereAnyCollected = true;
                coin.is_alive = false;
                t_context.audio.play("coin");
                t_context.info_region.scoreAdjust(1);
                ++t_context.stats.coin_collected;
                addAnimation(t_context, util::center(coinRect));
            }
        }

        if (wereAnyCollected)
        {
            std::erase_if(m_coins, [](const Coin & t_coin) { return !t_coin.is_alive; });
        }
    }

    void Coins::addAnimation(const Context & t_context, const sf::Vector2f & t_position)
    {
        if (!t_context.settings.will_show_coin_animations)
        {
            return;
        }

        CoinAnim & anim{ m_animations.emplace_back(m_texture) };
        anim.sprite.setTextureRect(m_textureCoords.at(0));
        util::setOriginToCenter(anim.sprite);
        anim.sprite.setPosition(t_position);
    }

} // namespace halloween
