// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// flys.cpp
//
#include "flys.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "filesystem-util.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "settings.hpp"
#include "texture-loader.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{

    FlyObjectManager::FlyObjectManager()
        : m_flys{}
        , m_texturesVecVec{}
    {}

    void FlyObjectManager::setup(const Context & t_context)
    {
        m_texturesVecVec.clear();

        const std::size_t typeCount{ static_cast<std::size_t>(FlyType::Count) };
        m_texturesVecVec.reserve(typeCount); // prevent any reallocations

        for (std::size_t typeIndex{ 0 }; typeIndex < typeCount; ++typeIndex)
        {
            const FlyType type{ static_cast<FlyType>(typeIndex) };

            std::vector<std::vector<sf::Texture>> & textureActions{
                m_texturesVecVec.emplace_back()
            };

            const std::size_t actionCount{ static_cast<std::size_t>(FlyAnim::Count) };
            textureActions.reserve(actionCount); // prevent any reallocations

            for (std::size_t actionIndex{ 0 }; actionIndex < actionCount; ++actionIndex)
            {
                const FlyAnim action{ static_cast<FlyAnim>(actionIndex) };

                const auto path{ t_context.settings.media_path / "image" / "fly" / toString(type) /
                                 toString(action) };

                const auto imagePaths{ util::findFilesInDirectory(path, ".png") };

                M_CHECK(not imagePaths.empty(), "No images to load found in " << path.string());

                std::vector<sf::Texture> & textures{ textureActions.emplace_back() };

                textures.reserve(imagePaths.size()); // prevent any reallocations

                for (std::size_t pathIndex{ 0 }; pathIndex < imagePaths.size(); ++pathIndex)
                {
                    sf::Texture & texture{ textures.emplace_back() };
                    util::TextureLoader::load(texture, imagePaths.at(pathIndex), true);
                }
            }
        }
    }

    void FlyObjectManager::add(
        const Context & t_context, const sf::FloatRect & t_region, const std::string &)
    {
        m_flys.emplace_back(t_context, t_region, m_texturesVecVec);
    }

    void FlyObjectManager::update(const Context & t_context, const float t_frameTimeSec)
    {
        for (Fly & fly : m_flys)
        {
            fly.update(t_context, t_frameTimeSec);
        }
    }

    void FlyObjectManager::moveWithMap(const sf::Vector2f & t_move)
    {
        for (Fly & fly : m_flys)
        {
            fly.moveWithMap(t_move);
        }
    }

    bool FlyObjectManager::doesAvatarCollideWithAnyAndDie(
        const Context & t_context, const sf::FloatRect & t_avatarRect)
    {
        for (Fly & fly : m_flys)
        {
            if (fly.doesAvatarCollideWithAnyAndDie(t_context, t_avatarRect))
            {
                return true;
            }
        }

        return false;
    }

    void FlyObjectManager::appendCollisions(std::vector<sf::FloatRect> & t_rects) const
    {
        for (const Fly & fly : m_flys)
        {
            if (fly.isAlive())
            {
                t_rects.push_back(fly.collisionRect());
            }
        }
    }

    void FlyObjectManager::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const Fly & fly : m_flys)
        {
            fly.draw(t_context, t_target, t_states);
        }
    }

    const Harm
        FlyObjectManager::attack(const Context & t_context, const sf::FloatRect & t_attackRect)
    {
        Harm harm;

        for (Fly & fly : m_flys)
        {
            if (fly.isAlive() and fly.collisionRect().findIntersection(t_attackRect))
            {
                harm.did_hit = true;
                fly.hit(t_context);

                if (not fly.isAlive())
                {
                    harm.did_kill = true;

                    ++t_context.stats.enemy_killed;
                    t_context.info_region.scoreAdjust(t_context.settings.kill_fly_score);
                }
            }
        }

        return harm;
    }

} // namespace halloween
