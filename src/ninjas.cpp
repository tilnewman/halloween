// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// ninjas.cpp
//
#include "ninjas.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "filesystem-util.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"
#include "texture-loader.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    Ninja::Ninja(
        const Context & t_context, const sf::Texture & t_texture, const sf::FloatRect & t_rect)
        : sprite{ t_texture }
        , anim_elapsed_sec{ 0.0f }
        , frame_index{ 0 }
    {
        util::setOriginToCenter(sprite);

        const float scale{ 0.375f };
        sprite.setScale({ scale, scale });

        sprite.setPosition(
            { util::center(t_rect).x,
              (util::bottom(t_rect) - (sprite.getGlobalBounds().size.y * 0.5f)) });

        if (t_context.random.boolean())
        {
            sprite.scale({ -1.0f, 1.0f });
        }
    }

    //

    NinjaObjectManager::NinjaObjectManager()
        : m_ninjas{}
        , m_textures{}
    {}

    void NinjaObjectManager::setup(const Context & t_context)
    {
        m_textures.clear();

        const auto imagePath{ t_context.settings.media_path / "image" / "ninja" };
        const auto imagePaths{ util::findFilesInDirectory(imagePath, ".png") };
        M_CHECK(not imagePaths.empty(), "No ninja images to load found in " << imagePath.string());
        m_textures.reserve(imagePaths.size()); // prevent any reallocations

        for (const auto & path : imagePaths)
        {
            util::TextureLoader::load(m_textures.emplace_back(), path);
        }
    }

    void NinjaObjectManager::add(
        const Context & t_context, const sf::FloatRect & t_rect, const std::string &)
    {
        m_ninjas.emplace_back(t_context, m_textures.at(0), t_rect);
    }

    void NinjaObjectManager::update(const Context &, const float m_frameTimeSec) 
    {
        for (Ninja & ninja : m_ninjas)
        {
            ninja.anim_elapsed_sec += m_frameTimeSec;
            const float timePerFrameSec{ 0.08f };
            if (ninja.anim_elapsed_sec > timePerFrameSec)
            {
                ninja.anim_elapsed_sec -= timePerFrameSec;

                if (++ninja.frame_index >= m_textures.size())
                {
                    ninja.frame_index = 0;
                }

                ninja.sprite.setTexture(m_textures.at(ninja.frame_index));
            }
        }
    }

    void NinjaObjectManager::moveWithMap(const sf::Vector2f & t_move)
    {
        for (Ninja & ninja : m_ninjas)
        {
            ninja.sprite.move(t_move);
        }
    }

    void NinjaObjectManager::appendCollisions(std::vector<sf::FloatRect> & t_rects) const
    {
        for (const Ninja & ninja : m_ninjas)
        {
            t_rects.push_back(ninja.sprite.getGlobalBounds());
        }
    }

    void NinjaObjectManager::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const Ninja & ninja : m_ninjas)
        {
            t_target.draw(ninja.sprite, t_states);
        }
    }

    bool NinjaObjectManager::doesAvatarCollideWithAnyAndDie(const Context &, const sf::FloatRect &)
    {
        return false;
    }

    const Harm NinjaObjectManager::attack(const Context &, const sf::FloatRect &) { return {}; }

} // namespace halloween
