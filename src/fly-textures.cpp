// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// fly-textures.cpp
//
#include "fly-textures.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "filesystem-util.hpp"
#include "settings.hpp"
#include "texture-loader.hpp"

#include <iostream>

namespace halloween
{

    FlyTextureManager::FlyTextureManager()
        : m_texturesVec{}
    {}

    void FlyTextureManager::setup(const Context & t_context)
    {
        m_texturesVec.clear();

        const std::size_t typeCount{ static_cast<std::size_t>(FlyType::Count) };
        m_texturesVec.reserve(typeCount); // prevent any reallocations

        for (std::size_t typeIndex{ 0 }; typeIndex < typeCount; ++typeIndex)
        {
            const FlyType type{ static_cast<FlyType>(typeIndex) };
            std::vector<std::vector<sf::Texture>> & textureActions{ m_texturesVec.emplace_back() };

            const std::size_t actionCount{ static_cast<std::size_t>(FlyAnim::Count) };
            textureActions.reserve(actionCount); // prevent any reallocations

            for (std::size_t actionIndex{ 0 }; actionIndex < actionCount; ++actionIndex)
            {
                const FlyAnim action{ static_cast<FlyAnim>(actionIndex) };

                const auto path{ t_context.settings.media_path / "image" / "fly" /
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

    void FlyTextureManager::teardown() { m_texturesVec.clear(); }

    const std::vector<sf::Texture> &
        FlyTextureManager::textures(const FlyType t_type, const FlyAnim t_action) const
    {
        const std::size_t typeIndex{ static_cast<std::size_t>(t_type) };

        M_CHECK(
            (typeIndex < m_texturesVec.size()),
            "textures(" << toString(t_type) << ", " << toString(t_action)
                        << ") when t_type=" << typeIndex << " is out of range!");

        const std::vector<std::vector<sf::Texture>> & textureActions{ m_texturesVec.at(typeIndex) };

        const std::size_t actionIndex{ static_cast<std::size_t>(t_action) };

        M_CHECK(
            (actionIndex < textureActions.size()),
            "textures(" << toString(t_type) << ", " << toString(t_action)
                        << ") when t_action=" << actionIndex << " is out of range!");

        return textureActions.at(actionIndex);
    }

} // namespace halloween
