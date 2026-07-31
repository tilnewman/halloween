// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// tileset.cpp
//
#include "tileset.hpp"

#include "texture-loader.hpp"

namespace halloween
{

    MapTexture::MapTexture()
        : which{ TileImage::Ground } // any default works here
        , size{}
        , texture{}
        , gid{ 0 } // any default works here
    {}

    void MapTexture::setup(const TileImage t_image, const std::filesystem::path & t_mapPath)
    {
        which = t_image;
        util::TextureLoader::load(texture, ((t_mapPath / toString(which)).string() + ".png"));
        size = sf::Vector2i{ texture.getSize() };

        // see LevelFileLoader.cpp for where gid gets set}
    }

} // namespace halloween
