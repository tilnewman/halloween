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
        , m_mapPosition{}
        , m_tileSizeScreen{}
        , m_walkCollisions{}
        , m_killCollisions{}
        , m_acidCollisions{}
        , m_waterCollisions{}
        , m_enterRect{}
        , m_exitRect{}
        , m_farthestHorizMapPixel{ 0.0f }
        , m_farthestHorizTraveled{ 0.0f }
        , m_number{ 0 }
        , m_loader{}
        , m_traveledVert{ 0.0f }
    {
        m_walkCollisions.reserve(1000);
        m_killCollisions.reserve(100);
        m_acidCollisions.reserve(100);
        m_waterCollisions.reserve(100);
    }

    void Level::reset(const Context & t_context)
    {
        m_tiles.reset();
        m_walkCollisions.clear();
        m_killCollisions.clear();
        m_acidCollisions.clear();
        m_waterCollisions.clear();
        t_context.managers.clearAll();
        m_farthestHorizMapPixel = 0.0f;
        m_farthestHorizTraveled = 0.0f;
        m_traveledVert = 0.0f;
    }

    bool Level::load(const Context & t_context)
    {
        reset(t_context);

        if (m_loader.load(t_context))
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

    bool Level::move(const Context & t_context, const sf::Vector2f & t_moveOrig)
    {
        sf::Vector2f move{ 0.0f, 0.0f };

        // the level can progress right but not left
        m_farthestHorizTraveled += std::abs(t_moveOrig.x);
        if (m_farthestHorizTraveled <
            (m_farthestHorizMapPixel - t_context.layout.mapRegion().size.x))
        {
            move.x = t_moveOrig.x;
        }

        if (t_moveOrig.y > 0.0f)
        {
            m_traveledVert += t_moveOrig.y;
            move.y = t_moveOrig.y;
        }
        else if ((t_moveOrig.y < 0.0f) && (m_traveledVert > 0.0f))
        {
            m_traveledVert += t_moveOrig.y;
            move.y = t_moveOrig.y;
        }

        if ((move.x < 0.0f) || (move.x > 0.0f) || (move.y < 0.0f) || (move.y > 0.0f))
        {
            m_enterRect.position += move;
            m_exitRect.position += move;

            for (sf::FloatRect & rect : m_killCollisions)
            {
                rect.position += move;
            }

            for (sf::FloatRect & rect : m_walkCollisions)
            {
                rect.position += move;
            }

            for (sf::FloatRect & rect : m_acidCollisions)
            {
                rect.position += move;
            }

            for (sf::FloatRect & rect : m_waterCollisions)
            {
                rect.position += move;
            }

            for (TileLayer & layer : m_tiles.layers)
            {
                for (sf::Vertex & vertex : layer.verts)
                {
                    vertex.position += move;
                }
            }

            t_context.managers.moveAllWithMap(move);

            populateVisibleVerts(t_context.layout);
            return true;
        }
        else
        {
            return false;
        }
    }

    void Level::appendVertLayers(const Context & t_context)
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

                const sf::Vector2f screenPos{ sf::Vector2f(posX, posY) + m_mapPosition };
                const sf::FloatRect screenRect{ screenPos, t_tileSizeOnScreen };

                util::appendTriangleVerts(screenRect, textureRect, t_layer.verts);
            }
        }
    }

    void Level::populateVisibleVerts(const ScreenRegions & t_layout)
    {
        const sf::FloatRect mapRect{ t_layout.mapRegion() };

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

                if (mapRect.contains(topLeftVert.position) ||
                    mapRect.contains(topRightVert.position) ||
                    mapRect.contains(botRightVert.position) ||
                    mapRect.contains(botLeftVert.position))
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
