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
    }

    void Level::appendVertLayer(
        const sf::Vector2i & count,
        const sf::Vector2i & size,
        const sf::Vector2f & sizeOnScreen,
        const TileTexture & TEXTURE,
        TileLayer & layer) const
    {
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
                // calc tile image on screen rect
                const float posX = static_cast<float>(x * sizeOnScreenI.x);

                // calc tile image texture rect
                const int textureIndexOrig = (layer.indexes.at(textureIndex++));
                if (textureIndexOrig == 0)
                {
                    // zero means no image at this location
                    continue;
                }

                const int index = (textureIndexOrig - TEXTURE.gid);

                const sf::Vector2i textureTileCount = { (TEXTURE.size.x / size.x),
                                                        (TEXTURE.size.y / size.y) };

                const int texturePosX = ((index % textureTileCount.x) * size.x);
                const int texturePosY = ((index / textureTileCount.x) * size.y);
                const sf::Vector2i texturePos = { texturePosX, texturePosY };
                const sf::IntRect textureRect = { texturePos, size };

                const sf::Vector2f screenPos = (sf::Vector2f(posX, posY) + map_position_offset);
                const sf::FloatRect screenRect = { screenPos, sizeOnScreen };

                util::appendQuadVerts(screenRect, textureRect, layer.verts);
            }
        }
    }
} // namespace halloween
