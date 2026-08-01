// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// fire-spout-large.cpp
//
#include "fire-spout-large.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "filesystem-util.hpp"
#include "random.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "texture-loader.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <filesystem>

namespace halloween
{

    FireSpoutLargeAnim::FireSpoutLargeAnim(
        const Context &,
        const sf::Texture & t_spoutTexture,
        const sf::Texture & t_fireTexture,
        const std::size_t t_frameIndex,
        const sf::FloatRect & t_rect)
        : fire_sprite{ t_fireTexture }
        , spout_sprite{ t_spoutTexture }
        , elapsed_time_sec{ 0.0f }
        , frame_index{ t_frameIndex }
    {
        spout_sprite.setTextureRect({ { 72, 19 }, { 39, 13 } });

        const float spoutScale{ 1.75f };
        spout_sprite.setScale({ spoutScale, spoutScale });

        spout_sprite.setPosition(
            { (util::center(t_rect).x - util::center(spout_sprite.getGlobalBounds()).x),
              (util::bottom(t_rect) - spout_sprite.getGlobalBounds().size.y) });

        const float fireScale{ 1.0f };
        fire_sprite.setScale({ fireScale, fireScale });

        fire_sprite.setPosition(
            { (util::center(spout_sprite.getGlobalBounds()).x -
               (fire_sprite.getGlobalBounds().size.x * 0.5f)),
              (spout_sprite.getGlobalBounds().position.y - fire_sprite.getGlobalBounds().size.y) });

        fire_sprite.move({ 0.0f, (fire_sprite.getGlobalBounds().size.y * 0.15f) });
    }

    const sf::FloatRect FireSpoutLargeAnim::collisionRect() const
    {
        return util::scaleRectInPlaceCopy(fire_sprite.getGlobalBounds(), { 0.15f, 0.65f });
    }

    //

    FireSpoutLargeManager::FireSpoutLargeManager()
        : m_spoutTexture{}
        , m_fireTextures{}
        , m_anims{}
    {}

    void FireSpoutLargeManager::setup(const Context & t_context)
    {
        util::TextureLoader::load(
            m_spoutTexture, (t_context.settings.media_path / "image" / "fire-spouts.png"));

        const auto imagePaths{ util::findFilesInDirectory(
            (t_context.settings.media_path / "image" / "fire-large"), ".png") };

        m_fireTextures.reserve(imagePaths.size()); // prevent any reallocations

        for (const std::filesystem::path & path : imagePaths)
        {
            sf::Texture & texture{ m_fireTextures.emplace_back() };
            util::TextureLoader::load(texture, path.string());
        }

        M_CHECK(!m_fireTextures.empty(), "Unable to find and load any fire-spout-large textures!");
    }

    void FireSpoutLargeManager::add(
        const Context & t_context, const sf::FloatRect & t_rect, const std::string &)
    {
        const std::size_t frameIndex{ t_context.random.zeroToOneLessThan(m_fireTextures.size()) };

        m_anims.emplace_back(
            t_context, m_spoutTexture, m_fireTextures.at(frameIndex), frameIndex, t_rect);
    }

    void FireSpoutLargeManager::update(const Context &, const float t_frameTimeSec)
    {
        for (FireSpoutLargeAnim & anim : m_anims)
        {
            anim.elapsed_time_sec += t_frameTimeSec;
            const float timePerFrameSec{ 0.075f };
            if (anim.elapsed_time_sec > timePerFrameSec)
            {
                anim.elapsed_time_sec -= timePerFrameSec;

                if (++anim.frame_index >= m_fireTextures.size())
                {
                    anim.frame_index = 0;
                }

                anim.fire_sprite.setTexture(m_fireTextures.at(anim.frame_index));
            }
        }
    }

    void FireSpoutLargeManager::moveWithMap(const sf::Vector2f & t_move)
    {
        for (FireSpoutLargeAnim & anim : m_anims)
        {
            anim.fire_sprite.move(t_move);
            anim.spout_sprite.move(t_move);
        }
    }

    void FireSpoutLargeManager::collideWithAvatar(const Context &, const sf::FloatRect &) {}

    bool FireSpoutLargeManager::doesAvatarCollideWithAnyAndDie(
        const sf::FloatRect & t_avatarRect) const
    {
        for (const FireSpoutLargeAnim & anim : m_anims)
        {
            if (anim.collisionRect().findIntersection(t_avatarRect))
            {
                return true;
            }
        }

        return false;
    }

    void FireSpoutLargeManager::appendCollisions(std::vector<sf::FloatRect> & t_rects) const
    {
        for (const FireSpoutLargeAnim & anim : m_anims)
        {
            t_rects.push_back(anim.spout_sprite.getGlobalBounds());
        }
    }

    void FireSpoutLargeManager::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const FireSpoutLargeAnim & anim : m_anims)
        {
            t_target.draw(anim.fire_sprite, t_states);
            t_target.draw(anim.spout_sprite, t_states);
        }
    }

} // namespace halloween
