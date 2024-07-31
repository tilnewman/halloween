#ifndef LEVEL_HPP_INCLUDED
#define LEVEL_HPP_INCLUDED
//
// level.hpp
//
#include "tileset.hpp"

#include <vector>

#include <SFML/Graphics/Rect.hpp>

namespace halloween
{
    struct Context;
    struct TileTexture;
    class ScreenRegions;

    //
    struct Level
    {
        Level();

        bool load(Context & context);

        bool move(const ScreenRegions & layout, const float move);

        TileSet tiles;
        sf::Vector2f map_position_offset;
        sf::Vector2f tile_size_screen;
        sf::Vector2f tile_size_texture;
        std::vector<sf::FloatRect> walk_collisions;
        std::vector<sf::FloatRect> kill_collisions;
        sf::FloatRect enter_rect;
        sf::FloatRect exit_rect;
        float farthest_horiz_map_pixel;
        float farthest_horiz_traveled;
        std::size_t number;

      private:
        void findFarthestHorizMapPixel();

        void appendVertLayers(Context & context);

        void appendVertLayer(
            const sf::Vector2i & count,
            const sf::Vector2i & size,
            const sf::Vector2f & sizeOnScreen,
            const TileTexture & texture,
            TileLayer & layer) const;

        void populateVisibleVerts(const ScreenRegions & layout);

        void dumpInfo(const std::size_t levelNumber) const;
    };

} // namespace halloween

#endif // LEVEL_HPP_INCLUDED
