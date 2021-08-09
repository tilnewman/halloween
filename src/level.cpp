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
#include "util.hpp"

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

    void Level::load(Context & context, const std::size_t LEVEL_NUMBER)
    {
        context.loader.load(context, LEVEL_NUMBER);
        appendVertLayers(context);
        context.avatar.setPosition(enter_rect);
        findFarthestHorizMapPixel();
        farthest_horiz_traveled = 0.0f;
    }

    bool Level::move(const ScreenRegions & layout, const float MOVE)
    {
        farthest_horiz_traveled += std::abs(MOVE);
        if (farthest_horiz_traveled > (farthest_horiz_map_pixel - layout.mapRegion().width))
        {
            return false;
        }

        enter_rect.left += MOVE;
        exit_rect.left += MOVE;

        for (sf::FloatRect & rect : kill_collisions)
        {
            rect.left += MOVE;
        }

        for (sf::FloatRect & rect : walk_collisions)
        {
            rect.left += MOVE;
        }

        for (TileLayer & layer : tiles.layers)
        {
            for (sf::Vertex & vertex : layer.verts)
            {
                vertex.position.x += MOVE;
            }
        }

        return true;
    }

    void Level::findFarthestHorizMapPixel()
    {
        farthest_horiz_map_pixel = 0.0f;

        for (const TileLayer & LAYER : tiles.layers)
        {
            for (const sf::Vertex & VERTEX : LAYER.verts)
            {
                if (VERTEX.position.x > farthest_horiz_map_pixel)
                {
                    farthest_horiz_map_pixel = VERTEX.position.x;
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
        const sf::Vector2i & COUNT,
        const sf::Vector2i & SIZE,
        const sf::Vector2f & SIZE_ON_SCREEN,
        const TileTexture & TEXTURE,
        TileLayer & layer) const
    {
        const std::size_t TOTAL_COUNT =
            (static_cast<std::size_t>(COUNT.x) * static_cast<std::size_t>(COUNT.y));

        M_CHECK(
            (TOTAL_COUNT == layer.indexes.size()),
            "index_count=" << layer.indexes.size() << " does not equal tile_count=" << TOTAL_COUNT);

        sf::Vector2i SIZE_ON_SCREENI(SIZE_ON_SCREEN);

        std::size_t textureIndex = 0;
        for (int y(0); y < COUNT.y; ++y)
        {
            const auto POS_Y = static_cast<float>(y * SIZE_ON_SCREENI.y);

            for (int x(0); x < COUNT.x; ++x)
            {
                // calc tile image on screen rect
                const auto POS_X = static_cast<float>(x * SIZE_ON_SCREENI.x);

                // calc tile image texture rect
                const int TEXTURE_INDEX_ORIG = (layer.indexes.at(textureIndex++));
                if (TEXTURE_INDEX_ORIG == 0)
                {
                    // zero means no image at this location
                    continue;
                }

                const int TEXTURE_INDEX = (TEXTURE_INDEX_ORIG - TEXTURE.gid);

                const sf::Vector2i TEXTURE_TILE_COUNT = { (TEXTURE.size.x / SIZE.x),
                                                          (TEXTURE.size.y / SIZE.y) };

                const int TEXTURE_POS_X = ((TEXTURE_INDEX % TEXTURE_TILE_COUNT.x) * SIZE.x);
                const int TEXTURE_POS_Y = ((TEXTURE_INDEX / TEXTURE_TILE_COUNT.x) * SIZE.y);
                const sf::Vector2i TEXTURE_POS = { TEXTURE_POS_X, TEXTURE_POS_Y };
                const sf::IntRect TEXTURE_RECT = { TEXTURE_POS, SIZE };

                const sf::Vector2f SCREEN_POS = (sf::Vector2f(POS_X, POS_Y) + map_position_offset);
                const sf::FloatRect SCREEN_RECT = { SCREEN_POS, SIZE_ON_SCREEN };

                util::appendQuadVerts(SCREEN_RECT, TEXTURE_RECT, layer.verts);
            }
        }
    }
} // namespace halloween
