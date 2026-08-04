// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// frogs.cpp
//
#include "frogs.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "filesystem-util.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "settings.hpp"
#include "texture-loader.hpp"

namespace halloween
{

    FrogObjectManager::FrogObjectManager()
        : m_frogs{}
        , m_texturesVec{}
    {}

    void FrogObjectManager::setup(const Context & t_context)
    {
        m_texturesVec.clear();

        const std::size_t actionCount{ static_cast<std::size_t>(FrogAnim::Count) };
        m_texturesVec.reserve(actionCount); // prevent any reallocations

        for (std::size_t actionIndex{ 0 }; actionIndex < actionCount; ++actionIndex)
        {
            const FrogAnim action{ static_cast<FrogAnim>(actionIndex) };

            const auto path{ t_context.settings.media_path / "image" / "frog" / toString(action) };
            const auto imagePaths{ util::findFilesInDirectory(path, ".png") };
            M_CHECK(not imagePaths.empty(), "No images to load found in " << path.string());

            std::vector<sf::Texture> & textures{ m_texturesVec.emplace_back() };
            textures.reserve(imagePaths.size()); // prevent any reallocations

            for (std::size_t pathIndex{ 0 }; pathIndex < imagePaths.size(); ++pathIndex)
            {
                sf::Texture & texture{ textures.emplace_back() };
                util::TextureLoader::load(texture, imagePaths.at(pathIndex), true);
            }
        }
    }

    void FrogObjectManager::add(
        const Context & t_context, const sf::FloatRect & t_region, const std::string &)
    {
        m_frogs.emplace_back(t_context, t_region, m_texturesVec);
    }

    void FrogObjectManager::update(const Context & t_context, const float t_frameTimeSec)
    {
        for (Frog & frog : m_frogs)
        {
            frog.update(t_context, t_frameTimeSec);
        }
    }

    void FrogObjectManager::moveWithMap(const sf::Vector2f & t_move)
    {
        for (Frog & frog : m_frogs)
        {
            frog.moveWithMap(t_move);
        }
    }

    bool FrogObjectManager::doesAvatarCollideWithAnyAndDie(
        const Context &, const sf::FloatRect & t_avatarRect)
    {
        for (const Frog & frog : m_frogs)
        {
            if (frog.doesAvatarCollideWithAnyAndDie(t_avatarRect))
            {
                return true;
            }
        }

        return false;
    }

    void FrogObjectManager::appendCollisions(std::vector<sf::FloatRect> & t_rects) const
    {
        for (const Frog & frog : m_frogs)
        {
            t_rects.push_back(frog.collisionRect());
        }
    }

    void FrogObjectManager::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const Frog & frog : m_frogs)
        {
            frog.draw(t_context, t_target, t_states);
        }
    }

    const Harm
        FrogObjectManager::attack(const Context & t_context, const sf::FloatRect & t_attackRect)
    {
        Harm harm;

        for (Frog & frog : m_frogs)
        {
            if (frog.isAlive() and frog.collisionRect().findIntersection(t_attackRect))
            {
                harm.did_hit = true;
                frog.hit(t_context);

                if (not frog.isAlive())
                {
                    harm.did_kill = true;

                    ++t_context.stats.enemy_killed;
                    t_context.info_region.scoreAdjust(t_context.settings.kill_frog_score);
                }
            }
        }

        return harm;
    }

} // namespace halloween
