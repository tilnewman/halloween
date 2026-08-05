#ifndef MAP_TEXTURES_HPP_INCLUDED
#define MAP_TEXTURES_HPP_INCLUDED
//
// map-textures.hpp
//
#include "tileset.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

namespace halloween
{

    struct Context;

    //

    struct MapTexture
    {
        MapTexture(const Context & t_context, const TileImage t_image);

        TileImage which;
        sf::Vector2i size;
        sf::Texture texture;
        int gid;
    };

    //

    class MapTextureManager
    {
      public:
        MapTextureManager();

        void setup(const Context & t_context);
        const MapTexture & mapTexture(const TileImage t_image) const;
        void setGid(const TileImage t_image, const int t_gid);

      private:
        std::vector<MapTexture> m_mapTextures;
    };

} // namespace halloween
#endif // MAP_TEXTURES_HPP_INCLUDED
