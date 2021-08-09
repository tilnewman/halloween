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
    //
    enum class TileImage
    {
        Ground = 0,
        Object1,
        Object2
    };

    inline std::ostream & operator<<(std::ostream & os, const TileImage IMAGE)
    {
        // clang-format off
        switch (IMAGE)
        {
            case TileImage::Ground: { os << "ground"; break; }
            case TileImage::Object1: { os << "object-1"; break; }
            case TileImage::Object2: { os << "object-2"; break; }
            default:                { assert(false);  break; }
        }
        // clang-format on

        return os;
    }

    //
    struct TileLayer
    {
        TileLayer()
            : image(TileImage::Ground) // any default works here
            , indexes()
            , verts()
        {}

        TileImage image;
        std::vector<int> indexes;
        std::vector<sf::Vertex> verts;
    };

    //
    struct TileSet
    {
        TileSet()
            : count()
            , size()
            , layers()
        {}

        sf::Vector2i count;
        sf::Vector2i size;
        std::vector<TileLayer> layers;
    };

} // namespace halloween

#endif // TILESET_HPP_INCLUDED
