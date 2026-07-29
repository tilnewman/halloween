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
#include "object-manager.hpp"
#include "resources.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"

#include <iostream>

namespace halloween
{

    Level::Level()
        : m_tiles{}
        , m_mapPositionOffset{}
        , m_tileSizeScreen{}
        , m_tileSizeTexture{}
        , m_walkCollisions{}
        , m_killCollisions{}
        , m_acidCollisions{}
        , m_waterCollisions{}
        , m_enterRect{}
        , m_exitRect{}
        , m_farthestHorizMapPixel{ 0.0f }
        , m_farthestHorizTraveled{ 0.0f }
        , m_number{ 0 }
    {
        m_walkCollisions.reserve(1000);
        m_killCollisions.reserve(100);
        m_acidCollisions.reserve(100);
        m_waterCollisions.reserve(100);
    }

    void Level::reset(Context & t_context)
    {
        m_tiles.reset();
        m_walkCollisions.clear();
        m_killCollisions.clear();
        m_acidCollisions.clear();
        m_waterCollisions.clear();
        t_context.managers.clearAll();
        m_farthestHorizMapPixel = 0.0f;
        m_farthestHorizTraveled = 0.0f;
    }

    bool Level::load(Context & t_context)
    {
        reset(t_context);
        
        if (t_context.loader.load(t_context))
        {
            verifyLayerIndexCounts();
            appendVertLayers(t_context);
            t_context.avatar.setSpawnPosForNewLevel(m_enterRect);
            m_farthestHorizMapPixel = m_exitRect.position.x;
            m_farthestHorizTraveled = 0.0f;
            // dumpInfo(levelNumber);
            return true;
        }
        else
        {
            return false;
        }
    }

    void Level::verifyLayerIndexCounts() const
    {
        for (const TileLayer & layer : m_tiles.layers)
        {
            const std::size_t totalCount{ static_cast<std::size_t>(m_tiles.count.x) *
                                          static_cast<std::size_t>(m_tiles.count.y) };

            M_CHECK(
                (totalCount == layer.indexes.size()),
                "index_count=" << layer.indexes.size() << " does not equal tile_count="
                               << totalCount << " in layer " << layer.image);

            M_CHECK(
                ((layer.verts.size() % util::verts_per_quad) == 0),
                "Error:  TileLayer " << layer.image << " verts.size()=" << layer.verts.size()
                                     << " which is not a multiple of " << util::verts_per_quad);

        }
    }

    bool Level::move(const ScreenRegions & t_layout, const float t_move)
    {
        m_farthestHorizTraveled += std::abs(t_move);
        if (m_farthestHorizTraveled > (m_farthestHorizMapPixel - t_layout.mapRegion().size.x))
        {
            return false;
        }

        m_enterRect.position.x += t_move;
        m_exitRect.position.x += t_move;

        for (sf::FloatRect & rect : m_killCollisions)
        {
            rect.position.x += t_move;
        }

        for (sf::FloatRect & rect : m_walkCollisions)
        {
            rect.position.x += t_move;
        }

        for (sf::FloatRect & rect : m_acidCollisions)
        {
            rect.position.x += t_move;
        }

        for (sf::FloatRect & rect : m_waterCollisions)
        {
            rect.position.x += t_move;
        }

        for (TileLayer & layer : m_tiles.layers)
        {
            for (sf::Vertex & vertex : layer.verts)
            {
                vertex.position.x += t_move;
            }
        }

        populateVisibleVerts(t_layout);

        return true;
    }

    void Level::appendVertLayers(Context & t_context)
    {
        for (TileLayer & layer : m_tiles.layers)
        {
            appendVertLayer(
                m_tiles.count,
                m_tiles.size,
                m_tileSizeScreen,
                t_context.media.tileTexture(layer.image),
                layer);
        }

        populateVisibleVerts(t_context.layout);
    }

    void Level::appendVertLayer(
        const sf::Vector2i & t_tileCount,
        const sf::Vector2i & t_tileSizeOnMap,
        const sf::Vector2f & t_tileSizeOnScreen,
        const TileTexture & t_tileTexture,
        TileLayer & t_layer) const
    {
        const sf::Vector2i sizeOnScreenI{ t_tileSizeOnScreen };
        const sf::Vector2i textureTileCount{ t_tileTexture.size / m_tiles.size };

        std::size_t textureIndex{ 0 };
        for (int y{ 0 }; y < t_tileCount.y; ++y)
        {
            for (int x{ 0 }; x < t_tileCount.x; ++x)
            {
                const int textureIndexOrig{ t_layer.indexes[textureIndex++] };
                if (textureIndexOrig == 0)
                {
                    continue; // zero means no image at this location
                }

                const int index{ textureIndexOrig - t_tileTexture.gid };

                const int texturePosX{ (index % textureTileCount.x) * t_tileSizeOnMap.x };
                const int texturePosY{ (index / textureTileCount.x) * t_tileSizeOnMap.y };

                const sf::Vector2i texturePos{ texturePosX, texturePosY };
                const sf::IntRect textureRect{ texturePos, t_tileSizeOnMap };

                const float posX{ static_cast<float>(x * sizeOnScreenI.x) };
                const float posY{ static_cast<float>(y * sizeOnScreenI.y) };

                const sf::Vector2f screenPos{ sf::Vector2f(posX, posY) + m_mapPositionOffset };
                const sf::FloatRect screenRect{ screenPos, t_tileSizeOnScreen };

                util::appendTriangleVerts(screenRect, textureRect, t_layer.verts);
            }
        }
    }

    void Level::populateVisibleVerts(const ScreenRegions & t_layout)
    {
        for (TileLayer & layer : m_tiles.layers)
        {
            layer.visibleVerts.clear();

            std::size_t vertIndex{ 0 };
            while (vertIndex < layer.verts.size())
            {
                // see sfml-util::setupTriangleVerts() for where this order comes from
                const sf::Vertex & topLeftVert{ layer.verts[vertIndex + 0] };
                const sf::Vertex & topRightVert{ layer.verts[vertIndex + 1] };
                const sf::Vertex & botLeftVert{ layer.verts[vertIndex + 2] };
                const sf::Vertex & botLeftVert2{ layer.verts[vertIndex + 3] };
                const sf::Vertex & topRightVert2{ layer.verts[vertIndex + 4] };
                const sf::Vertex & botRightVert{ layer.verts[vertIndex + 5] };

                if (t_layout.mapRegion().contains(topLeftVert.position) ||
                    t_layout.mapRegion().contains(topRightVert.position) ||
                    t_layout.mapRegion().contains(botRightVert.position) ||
                    t_layout.mapRegion().contains(botLeftVert.position))
                {
                    layer.visibleVerts.push_back(topLeftVert);
                    layer.visibleVerts.push_back(topRightVert);
                    layer.visibleVerts.push_back(botLeftVert);
                    layer.visibleVerts.push_back(botLeftVert2);
                    layer.visibleVerts.push_back(topRightVert2);
                    layer.visibleVerts.push_back(botRightVert);
                }

                vertIndex += util::verts_per_quad;
            }
        }
    }

    void Level::dumpInfo(const std::size_t levelNumber) const
    {
        std::cout << "Level " << levelNumber << " Graphics Info\n";

        for (const TileLayer & layer : m_tiles.layers)
        {
            std::cout << "\tLayer Tiles:  " << layer.image << ", possible=" << layer.indexes.size()
                      << ", actual=" << (layer.verts.size() / util::verts_per_quad)
                      << ", visible=" << (layer.visibleVerts.size() / util::verts_per_quad) << "\n";
        }

        std::cout << std::endl;
    }

} // namespace halloween
