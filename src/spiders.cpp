// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// spiders.cpp
//
#include "spiders.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "filesystem-util.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "settings.hpp"
#include "texture-loader.hpp"

namespace halloween
{

    SpiderObjectManager::SpiderObjectManager()
        : m_spiders{}
        , m_webTexture{}
        , m_texturesVecVec{}
    {}

    void SpiderObjectManager::setup(const Context & t_context)
    {
        util::TextureLoader::load(
            m_webTexture, (t_context.settings.media_path / "image" / "spiderweb.png"), true);

        m_texturesVecVec.clear();

        const std::size_t typeCount{ static_cast<std::size_t>(SpiderType::Count) };
        m_texturesVecVec.reserve(typeCount); // prevent any reallocations

        for (std::size_t typeIndex{ 0 }; typeIndex < typeCount; ++typeIndex)
        {
            const SpiderType type{ static_cast<SpiderType>(typeIndex) };

            std::vector<std::vector<sf::Texture>> & textureActions{
                m_texturesVecVec.emplace_back()
            };

            const std::size_t actionCount{ static_cast<std::size_t>(SpiderAnim::Count) };
            textureActions.reserve(actionCount); // prevent any reallocations

            for (std::size_t actionIndex{ 0 }; actionIndex < actionCount; ++actionIndex)
            {
                const SpiderAnim action{ static_cast<SpiderAnim>(actionIndex) };

                const auto path{ t_context.settings.media_path / "image" / "spider" /
                                 toString(type) / toString(action) };

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

    void SpiderObjectManager::add(
        const Context & t_context, const sf::FloatRect & t_region, const std::string &)
    {
        m_spiders.emplace_back(t_context, t_region, m_webTexture, m_texturesVecVec);
    }

    void SpiderObjectManager::update(const Context & t_context, const float t_frameTimeSec)
    {
        for (Spider & spider : m_spiders)
        {
            spider.update(t_context, t_frameTimeSec);
        }
    }

    void SpiderObjectManager::moveWithMap(const sf::Vector2f & t_move)
    {
        for (Spider & spider : m_spiders)
        {
            spider.moveWithMap(t_move);
        }
    }

    bool SpiderObjectManager::doesAvatarCollideWithAnyAndDie(
        const Context & t_context, const sf::FloatRect & t_avatarRect)
    {
        for (Spider & spider : m_spiders)
        {
            if (spider.doesAvatarCollideWithAnyAndDie(t_context, t_avatarRect))
            {
                return true;
            }
        }

        return false;
    }

    void SpiderObjectManager::appendCollisions(std::vector<sf::FloatRect> & t_rects) const
    {
        for (const Spider & spider : m_spiders)
        {
            if (spider.isAlive())
            {
                t_rects.push_back(spider.collisionRect());
            }
        }
    }

    void SpiderObjectManager::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const Spider & spider : m_spiders)
        {
            spider.draw(t_context, t_target, t_states);
        }
    }

    const Harm
        SpiderObjectManager::attack(const Context & t_context, const sf::FloatRect & t_attackRect)
    {
        Harm harm;

        for (Spider & spider : m_spiders)
        {
            if (spider.isAlive() and spider.collisionRect().findIntersection(t_attackRect))
            {
                harm.did_hit = true;
                spider.hit(t_context);

                if (not spider.isAlive())
                {
                    harm.did_kill = true;

                    ++t_context.stats.enemy_killed;
                    t_context.info_region.scoreAdjust(t_context.settings.kill_spider_score);
                }
            }
        }

        return harm;
    }

} // namespace halloween
