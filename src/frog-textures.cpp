// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// frog-textures.cpp
//
#include "frog-textures.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "filesystem-util.hpp"
#include "settings.hpp"
#include "texture-loader.hpp"

#include <iostream>

namespace halloween
{

    FrogTextureManager::FrogTextureManager()
        : m_texturesVec{}
    {}

    void FrogTextureManager::setup(const Context & t_context)
    {
        m_texturesVec.clear();

        const std::size_t actionCount{ static_cast<std::size_t>(FrogAnim::Count) };
        m_texturesVec.reserve(actionCount); // prevent any reallocations

        for (std::size_t actionIndex{ 0 }; actionIndex < actionCount; ++actionIndex)
        {
            const FrogAnim action{ static_cast<FrogAnim>(actionIndex) };

            const auto path{ t_context.settings.media_path / "image" / "frog" /
                             toString(action) };

            const auto imagePaths{ util::findFilesInDirectory(path, ".png") };

            M_CHECK(not imagePaths.empty(), "No images to load found in " << path.string());

            std::vector<sf::Texture> & textures{ m_texturesVec.emplace_back() };

            textures.reserve(imagePaths.size()); // prevent any reallocations

            for (std::size_t pathIndex{ 0 }; pathIndex < imagePaths.size(); ++pathIndex)
            {
                sf::Texture & texture{ textures.emplace_back() };
                util::TextureLoader::load(texture, imagePaths.at(pathIndex));
            }
        }
    }

    void FrogTextureManager::teardown()
    {
        for (std::vector<sf::Texture> & textures : m_texturesVec)
        {
            textures.clear();
        }
    }

    const std::vector<sf::Texture> & FrogTextureManager::textures(const FrogAnim t_action) const
    {
        const std::size_t actionIndex{ static_cast<std::size_t>(t_action) };

        M_CHECK(
            (actionIndex < m_texturesVec.size()),
            "textures(" << toString(t_action) << ") when t_action=" << toString(t_action)
                        << " is out of range!");

        return m_texturesVec.at(actionIndex);
    }

} // namespace halloween
