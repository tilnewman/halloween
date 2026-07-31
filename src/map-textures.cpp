
// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// map-textures.cpp
//
#include "map-textures.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "settings.hpp"
#include "texture-loader.hpp"

namespace halloween
{

    MapTexture::MapTexture(const Context & t_context, const TileImage t_image)
        : which{ t_image }
        , size{}
        , texture{}
        , gid{ 0 } // any default works here, see LevelFileLoader.cpp for where gid gets set
    {
        util::TextureLoader::load(
            texture,
            ((t_context.settings.media_path / "image" / "map" / toString(which)).string() +
             ".png"));

        size = sf::Vector2i{ texture.getSize() };
    }

    //

    MapTextureManager::MapTextureManager()
        : m_mapTextures{}
    {}

    void MapTextureManager::setup(const Context & t_context)
    {
        const std::size_t tileImageCount{ static_cast<std::size_t>(TileImage::Count) };
        m_mapTextures.reserve(tileImageCount); // prevent any reallocations

        for (std::size_t i{ 0 }; i < tileImageCount; ++i)
        {
            m_mapTextures.emplace_back(t_context, static_cast<TileImage>(i));
        }
    }

    const MapTexture & MapTextureManager::mapTexture(const TileImage t_image) const
    {
        M_CHECK((TileImage::Count != t_image), "TileImage enum was Count!");
        const std::size_t index{ static_cast<std::size_t>(t_image) };

        M_CHECK(
            (index < m_mapTextures.size()),
            "TileImage enum " << toString(t_image) << " was out of range!");

        return m_mapTextures.at(index);
    }

    void MapTextureManager::setGid(const TileImage t_image, const int t_gid)
    {
        M_CHECK((TileImage::Count != t_image), "TileImage enum was Count!");
        const std::size_t index{ static_cast<std::size_t>(t_image) };

        M_CHECK(
            (index < m_mapTextures.size()),
            "TileImage enum " << toString(t_image) << " was out of range!");

        m_mapTextures.at(index).gid = t_gid;
    }

} // namespace halloween
