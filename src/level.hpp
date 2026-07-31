#ifndef LEVEL_HPP_INCLUDED
#define LEVEL_HPP_INCLUDED
//
// level.hpp
//
#include "level-file-loader.hpp"
#include "tileset.hpp"

#include <vector>

#include <SFML/Graphics/Rect.hpp>

namespace halloween
{
    struct Context;
    struct TileTexture;
    class ScreenRegions;

    //
    class Level
    {
      public:
        Level();

        bool load(const Context & t_context);
        bool move(const Context & t_context, const sf::Vector2f & t_move);

        inline const std::vector<sf::FloatRect> & walkCollisions() const
        {
            return m_walkCollisions;
        }

        void setWalkCollisions(const std::vector<sf::FloatRect> & t_rects)
        {
            m_walkCollisions = t_rects;
        }

        inline const std::vector<sf::FloatRect> & killCollisions() const
        {
            return m_killCollisions;
        }

        void setKillCollisions(const std::vector<sf::FloatRect> & t_rects)
        {
            m_killCollisions = t_rects;
        }

        inline const std::vector<sf::FloatRect> & acidCollisions() const
        {
            return m_acidCollisions;
        }

        void setAcidCollisions(const std::vector<sf::FloatRect> & t_rects)
        {
            m_acidCollisions = t_rects;
        }

        inline const std::vector<sf::FloatRect> & waterCollisions() const
        {
            return m_waterCollisions;
        }

        void setWaterCollisions(const std::vector<sf::FloatRect> & t_rects)
        {
            m_waterCollisions = t_rects;
        }

        inline const std::vector<TileLayer> & tileLayers() const { return m_mapTiles.layers; }

        void appendTileLayer(const TileLayer & t_tileLayer)
        {
            m_mapTiles.layers.push_back(t_tileLayer);
        }

        inline const sf::Vector2f mapPosition() const { return m_mapPosition; }
        inline const sf::FloatRect & enterRect() const { return m_enterRect; }
        inline const sf::FloatRect & exitRect() const { return m_exitRect; }

        constexpr std::size_t number() const noexcept { return m_number; }
        constexpr void numberIncrement() noexcept { ++m_number; }

        inline void setEnterAndExitRects(
            const sf::FloatRect & t_enterRect, const sf::FloatRect & t_exitRect)
        {
            m_enterRect = t_enterRect;
            m_exitRect = t_exitRect;
        }

        inline void setLevelDetails(
            const sf::Vector2i & t_tileCount,
            const sf::Vector2i & t_tileSizeOnMap,
            const sf::Vector2f & t_tileSizeScreen,
            const sf::Vector2f & t_mapPosition)
        {
            m_mapTiles.count = t_tileCount;
            m_mapTiles.size = t_tileSizeOnMap;
            m_tileSizeScreen = t_tileSizeScreen;
            m_mapPosition = t_mapPosition;
        }

      private:
        void reset(const Context & t_context);
        void appendVertLayers(const Context & t_context);
        void populateVisibleVerts(const ScreenRegions & t_layout);
        void dumpInfo(const std::size_t t_levelNumber) const;
        void verifyLayerIndexCounts() const;

        void appendVertLayer(
            const sf::Vector2i & t_tileCount,
            const sf::Vector2i & t_tileSizeOnMap,
            const sf::Vector2f & t_tileSizeOnScreen,
            const TileTexture & t_tileTexture,
            TileLayer & t_layer) const;

      private:
        MapTiles m_mapTiles;
        sf::Vector2f m_mapPosition;
        sf::Vector2f m_tileSizeScreen;
        std::vector<sf::FloatRect> m_walkCollisions;
        std::vector<sf::FloatRect> m_killCollisions;
        std::vector<sf::FloatRect> m_acidCollisions;
        std::vector<sf::FloatRect> m_waterCollisions;
        sf::FloatRect m_enterRect;
        sf::FloatRect m_exitRect;
        float m_farthestHorizMapPixel;
        float m_farthestHorizTraveled;
        std::size_t m_number;
        LevelFileLoader m_loader;
        float m_traveledVert;
    };

} // namespace halloween

#endif // LEVEL_HPP_INCLUDED
