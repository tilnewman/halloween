#ifndef TILESET_HPP_INCLUDED
#define TILESET_HPP_INCLUDED
//
// tileset.hpp
//
#include <cassert>
#include <ostream>
#include <string>
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>

namespace halloween
{

    enum class TileImage
    {
        Ground = 0,
        Object1,
        Object2
    };

    inline std::ostream & operator<<(std::ostream & os, const TileImage image)
    {
        // clang-format off
        switch (image)
        {
            case TileImage::Ground:  { os << "ground";   break; }
            case TileImage::Object1: { os << "object-1"; break; }
            case TileImage::Object2: { os << "object-2"; break; }
            default:                 { assert(false);    break; }
        }
        // clang-format on

        return os;
    }

    //

    struct TileLayer
    {
        TileLayer()
            : image(TileImage::Ground) // any works here
            , indexes()
            , verts()
            , visibleVerts()
        {
            indexes.reserve(10'000);
            verts.reserve(10'000);
            visibleVerts.reserve(10'000);
        }

        TileImage image;
        std::vector<int> indexes;
        std::vector<sf::Vertex> verts;
        std::vector<sf::Vertex> visibleVerts;
    };

    //

    struct TileSet
    {
        TileSet()
            : count()
            , size()
            , layers()
        {
            layers.reserve(16);
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
