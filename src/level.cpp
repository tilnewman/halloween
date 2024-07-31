// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// level.cpp
//
#include "level.hpp"

#include "avatar.hpp"
#include "check-macros.hpp"
#include "context.hpp"
#include "level-file-loader.hpp"
#include "resources.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"

#include <iostream>

namespace halloween
{

    Level::Level()
        : tiles()
        , map_position_offset()
        , tile_size_screen()
        , tile_size_texture()
        , walk_collisions()
        , kill_collisions()
        , enter_rect()
        , exit_rect()
        , farthest_horiz_map_pixel(0.0f)
        , farthest_horiz_traveled(0.0f)
    {}

    void Level::load(Context & context, const std::size_t levelNumber)
    {
        context.loader.load(context, levelNumber);
        appendVertLayers(context);
        context.avatar.setPosition(enter_rect);
        findFarthestHorizMapPixel();
        farthest_horiz_traveled = 0.0f;
        // dumpInfo(levelNumber);
    }

    bool Level::move(const ScreenRegions & layout, const float move)
    {
        farthest_horiz_traveled += std::abs(move);
        if (farthest_horiz_traveled > (farthest_horiz_map_pixel - layout.mapRegion().width))
        {
            return false;
        }

        enter_rect.left += move;
        exit_rect.left += move;

        for (sf::FloatRect & rect : kill_collisions)
        {
            rect.left += move;
        }

        for (sf::FloatRect & rect : walk_collisions)
        {
            rect.left += move;
        }

        for (TileLayer & layer : tiles.layers)
        {
            for (sf::Vertex & vertex : layer.verts)
            {
                vertex.position.x += move;
            }
        }

        populateVisibleVerts(layout);

        return true;
    }

    void Level::findFarthestHorizMapPixel()
    {
        farthest_horiz_map_pixel = 0.0f;

        for (const TileLayer & layer : tiles.layers)
        {
            for (const sf::Vertex & vertex : layer.verts)
            {
                if (vertex.position.x > farthest_horiz_map_pixel)
                {
                    farthest_horiz_map_pixel = vertex.position.x;
                }
            }
        }
    }

    void Level::appendVertLayers(Context & context)
    {
        for (TileLayer & layer : tiles.layers)
        {
            appendVertLayer(
                tiles.count,
                tiles.size,
                tile_size_screen,
                context.media.tileTexture(layer.image),
                layer);
        }

        populateVisibleVerts(context.layout);
    }

    void Level::appendVertLayer(
        const sf::Vector2i & count,
        const sf::Vector2i & size,
        const sf::Vector2f & sizeOnScreen,
        const TileTexture & texture,
        TileLayer & layer) const
    {
        const sf::Vector2i textureTileCount{ texture.size / size };

        const std::size_t totalCount =
            (static_cast<std::size_t>(count.x) * static_cast<std::size_t>(count.y));

        M_CHECK(
            (totalCount == layer.indexes.size()),
            "index_count=" << layer.indexes.size() << " does not equal tile_count=" << totalCount);

        const sf::Vector2i sizeOnScreenI(sizeOnScreen);

        std::size_t textureIndex = 0;
        for (int y(0); y < count.y; ++y)
        {
            const float posY = static_cast<float>(y * sizeOnScreenI.y);

            for (int x(0); x < count.x; ++x)
            {
                const float posX = static_cast<float>(x * sizeOnScreenI.x);

                const int textureIndexOrig(layer.indexes[textureIndex++]);
                if (textureIndexOrig == 0)
                {
                    continue; // zero means no image at this location
                }

                const int index(textureIndexOrig - texture.gid);

                const int texturePosX((index % textureTileCount.x) * size.x);
                const int texturePosY((index / textureTileCount.x) * size.y);

                const sf::Vector2i texturePos{ texturePosX, texturePosY };
                const sf::IntRect textureRect{ texturePos, size };

                const sf::Vector2f screenPos(sf::Vector2f(posX, posY) + map_position_offset);
                const sf::FloatRect screenRect{ screenPos, sizeOnScreen };

                util::appendQuadVerts(screenRect, textureRect, layer.verts);
            }
        }
    }

    void Level::populateVisibleVerts(const ScreenRegions & layout)
    {
        for (TileLayer & layer : tiles.layers)
        {
            M_CHECK(
                ((layer.verts.size() % util::verts_per_quad) == 0),
                "Error:  TileLayer.verts.size()=" << layer.verts.size()
                                                  << " which is not a multiple of "
                                                  << util::verts_per_quad);

            layer.visibleVerts.clear();

            std::size_t vertIndex = 0;
            while (vertIndex < layer.verts.size())
            {
                const sf::Vertex topLeftVert = layer.verts[vertIndex++];
                const sf::Vertex topRightVert = layer.verts[vertIndex++];
                const sf::Vertex botLeftVert = layer.verts[vertIndex++];
                const sf::Vertex botRightVert = layer.verts[vertIndex++];

                if (layout.mapRegion().contains(topLeftVert.position) ||
                    layout.mapRegion().contains(botRightVert.position))
                {
                    layer.visibleVerts.push_back(topLeftVert);
                    layer.visibleVerts.push_back(topRightVert);
                    layer.visibleVerts.push_back(botLeftVert);
                    layer.visibleVerts.push_back(botRightVert);
                }
            }
        }
    }

    void Level::dumpInfo(const std::size_t levelNumber) const
    {
        std::cout << "Level " << levelNumber << " Graphics Info\n";

        for (const TileLayer & layer : tiles.layers)
        {
            std::cout << "\tLayer Tiles:  " << layer.image << ", possible=" << layer.indexes.size()
                      << ", actual=" << (layer.verts.size() / util::verts_per_quad)
                      << ", visible=" << (layer.visibleVerts.size() / util::verts_per_quad) << "\n";
        }

        std::cout << std::endl;
    }

} // namespace halloween
