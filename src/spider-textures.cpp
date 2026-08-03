// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// spider-textures.cpp
//
#include "spider-textures.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "filesystem-util.hpp"
#include "settings.hpp"
#include "texture-loader.hpp"

#include <iostream>

namespace halloween
{

    SpiderTextureManager::SpiderTextureManager()
        : m_webTexture{}
        , m_texturesVec{}
    {}

    void SpiderTextureManager::setup(const Context & t_context)
    {
        util::TextureLoader::load(
            m_webTexture, (t_context.settings.media_path / "image" / "spiderweb.png"), true);

        m_texturesVec.clear();

        const std::size_t typeCount{ static_cast<std::size_t>(SpiderType::Count) };
        m_texturesVec.reserve(typeCount); // prevent any reallocations

        for (std::size_t typeIndex{ 0 }; typeIndex < typeCount; ++typeIndex)
        {
            const SpiderType type{ static_cast<SpiderType>(typeIndex) };
            std::vector<std::vector<sf::Texture>> & textureActions{ m_texturesVec.emplace_back() };

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

    void SpiderTextureManager::teardown() { m_texturesVec.clear(); }

    const std::vector<sf::Texture> &
        SpiderTextureManager::textures(const SpiderType t_type, const SpiderAnim t_action) const
    {
        const std::size_t typeIndex{ static_cast<std::size_t>(t_type) };

        M_CHECK(
            (typeIndex < m_texturesVec.size()),
            "textures(" << toString(t_type) << ", " << toString(t_action)
                        << ") when t_type=" << typeIndex << " is out of range!");

        const std::size_t actionIndex{ static_cast<std::size_t>(t_action) };

        M_CHECK(
            (actionIndex < m_texturesVec.size()),
            "textures(" << toString(t_type) << ", " << toString(t_action)
                        << ") when t_action=" << actionIndex << " is out of range!");

        return m_texturesVec.at(typeIndex).at(actionIndex);
    }

} // namespace halloween
