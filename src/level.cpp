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
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <iostream>

namespace halloween
{

    Level::Level()
        : m_mapTiles{}
        , m_mapTextures{}
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
        , m_backgroundImage{ 0 }
    {
        m_walkCollisions.reserve(100);
        m_killCollisions.reserve(32);
        m_acidCollisions.reserve(32);
        m_waterCollisions.reserve(32);
    }

    void Level::setup(const Context & t_context) { m_mapTextures.setup(t_context); }

    void Level::reset(const Context & t_context)
    {
        m_mapTiles.reset();
        m_walkCollisions.clear();
        m_killCollisions.clear();
        m_acidCollisions.clear();
        m_waterCollisions.clear();
        t_context.managers.clear();
        m_farthestHorizMapPixel = 0.0f;
        m_farthestHorizTraveled = 0.0f;
        m_traveledVert = 0.0f;
        m_backgroundImage = 0;
    }

    bool Level::load(const Context & t_context)
    {
        reset(t_context);

        if (m_loader.load(t_context))
        {
            verifyLayerIndexCounts();
            appendVertLayers(t_context);
            t_context.avatar.resetForNewLevel(m_enterRect);
            m_farthestHorizMapPixel = m_exitRect.position.x;
            m_farthestHorizTraveled = 0.0f;
            // dumpInfo(m_number);
            return true;
        }
        else
        {
            return false;
        }
    }

    void Level::verifyLayerIndexCounts() const
    {
        for (const TileLayer & layer : m_mapTiles.layers)
        {
            const std::size_t totalCount{ static_cast<std::size_t>(m_mapTiles.count.x) *
                                          static_cast<std::size_t>(m_mapTiles.count.y) };

            M_CHECK(
                (totalCount == layer.indexes.size()),
                "index_count=" << layer.indexes.size() << " does not equal tile_count="
                               << totalCount << " in layer " << toString(layer.image));

            M_CHECK(
                ((layer.verts.size() % util::verts_per_quad) == 0),
                "Error:  TileLayer " << toString(layer.image)
                                     << " verts.size()=" << layer.verts.size()
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

            for (TileLayer & layer : m_mapTiles.layers)
            {
                for (sf::Vertex & vertex : layer.verts)
                {
                    vertex.position += move;
                }
            }

            t_context.managers.moveWithMap(move);

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
        for (TileLayer & layer : m_mapTiles.layers)
        {
            appendVertLayer(
                m_mapTiles.count,
                m_mapTiles.size,
                m_tileSizeScreen,
                m_mapTextures.mapTexture(layer.image),
                layer);
        }

        populateVisibleVerts(t_context.layout);
    }

    void Level::appendVertLayer(
        const sf::Vector2i & t_tileCount,
        const sf::Vector2i & t_tileSizeOnMap,
        const sf::Vector2f & t_tileSizeOnScreen,
        const MapTexture & t_mapTexture,
        TileLayer & t_layer) const
    {
        const sf::Vector2i sizeOnScreenI{ t_tileSizeOnScreen };
        const sf::Vector2i textureTileCount{ t_mapTexture.size / m_mapTiles.size };

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

                const int index{ textureIndexOrig - t_mapTexture.gid };

                const int texturePosX{ (index % textureTileCount.x) * t_tileSizeOnMap.x };
                const int texturePosY{ (index / textureTileCount.x) * t_tileSizeOnMap.y };

                const sf::IntRect textureRect{ { texturePosX, texturePosY }, t_tileSizeOnMap };

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

        for (TileLayer & layer : m_mapTiles.layers)
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

    void Level::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const TileLayer & layer : t_context.level.tileLayers())
        {
            if (layer.visibleVerts.empty())
            {
                continue;
            }

            t_states.texture = &m_mapTextures.mapTexture(layer.image).texture;

            t_target.draw(
                &layer.visibleVerts[0],
                layer.visibleVerts.size(),
                sf::PrimitiveType::Triangles,
                t_states);

            t_states.texture = nullptr;
        }
    }

    void Level::dumpInfo(const std::size_t levelNumber) const
    {
        std::cout << "Level " << levelNumber << " Graphics Info\n";

        for (const TileLayer & layer : m_mapTiles.layers)
        {
            std::cout << "\tLayer Tiles:  " << toString(layer.image)
                      << ", possible=" << layer.indexes.size()
                      << ", actual=" << (layer.verts.size() / util::verts_per_quad)
                      << ", visible=" << (layer.visibleVerts.size() / util::verts_per_quad) << "\n";
        }

        std::cout << std::endl;
    }

} // namespace halloween
