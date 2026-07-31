#ifndef TILESET_HPP_INCLUDED
#define TILESET_HPP_INCLUDED
//
// tileset.hpp
//
#include <cassert>
#include <ostream>
#include <string_view>
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Vertex.hpp>

namespace halloween
{

    enum class TileImage : unsigned char
    {
        Ground = 0,
        Object1,
        Object2,
        Object3,
        JungleTrees,
        JungleTreesFlip,
        JungleMisc,
        Count
    };

    // these names must match the layer names in Tiled, and the files in media/tiled
    constexpr const std::string_view toString(const TileImage t_image) noexcept
    {
        // clang-format off
        switch (t_image)
        {
            case TileImage::Ground:          { return "ground";             }
            case TileImage::Object1:         { return "object-1";           }
            case TileImage::Object2:         { return "object-2";           }
            case TileImage::Object3:         { return "object-3";           }
            case TileImage::JungleTrees:     { return "jungle-trees";       }
            case TileImage::JungleTreesFlip: { return "jungle-trees-flip";  }
            case TileImage::JungleMisc:      { return "jungle-misc";        }
            case TileImage::Count:
            default:                   { return "ERROR_Unknown_TileImage";  }
        }
        // clang-format on
    }

    //

    struct TileLayer
    {
        TileLayer()
            : image{ TileImage::Ground } // any works here
            , indexes{}
            , verts{}
            , visibleVerts{}
        {
            indexes.reserve(10'000);
            verts.reserve(2'000);
            visibleVerts.reserve(1'000);
        }

        TileImage image;
        std::vector<int> indexes;
        std::vector<sf::Vertex> verts;
        std::vector<sf::Vertex> visibleVerts;
    };

    //

    struct MapTiles
    {
        MapTiles()
            : count{}
            , size{}
            , layers{}
        {
            layers.reserve(32);
        }

        void reset()
        {
            layers.clear();
            count = { 0, 0 };
            size = { 0, 0 };
        }

        sf::Vector2i count;
        sf::Vector2i size;
        std::vector<TileLayer> layers;
    };

} // namespace halloween

#endif // TILESET_HPP_INCLUDED
