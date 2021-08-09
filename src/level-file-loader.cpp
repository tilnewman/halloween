// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "level-file-loader.hpp"

#include "check-macros.hpp"
#include "coin.hpp"
#include "context.hpp"
#include "level.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "util.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace slnghn
{

    LevelFileLoader::LevelFileLoader()
        : m_path()
    {}

    void LevelFileLoader::load(Context & context, const std::size_t LEVEL_NUMBER)
    {
        std::stringstream fileNameSS;
        fileNameSS << "level-" << LEVEL_NUMBER << ".json";

        m_path = (context.settings.media_path / "map" / fileNameSS.str()).string();

        // open file
        std::ifstream iStream(m_path);
        M_CHECK(iStream, "Failed to open level file: \"" << m_path << "\"");

        // stream file into json parser
        Json json;
        iStream >> json;

        parseLevelDetails(context, json);

        // everything else in the level file is saved in "layers"
        // which are parsed in order from back to front here, one at a time
        parseLayers(context, json);
    }

    void LevelFileLoader::parseLevelDetails(Context & context, Json & json)
    {
        // parse level tile size and counts
        context.level.tiles.count = { json["width"], json["height"] };
        context.level.tiles.size = { json["tilewidth"], json["tileheight"] };

        context.level.tile_size_texture = sf::Vector2f{ context.level.tiles.size };

        context.level.tile_size_screen =
            (sf::Vector2f{ context.level.tiles.size } * context.settings.tile_scale);

        context.level.tile_size_screen.x = floorf(context.level.tile_size_screen.x);
        context.level.tile_size_screen.y = floorf(context.level.tile_size_screen.y);

        // calc map position offset
        const sf::Vector2f TILE_COUNTF{ context.level.tiles.count };
        const sf::Vector2f MAP_SIZE_ORIG{ context.level.tile_size_screen * TILE_COUNTF };

        const float HEIGHT_OFFSET{
            (context.layout.mapRegion().top + context.layout.mapRegion().height) - MAP_SIZE_ORIG.y
        };

        context.level.map_position_offset = { 0.0f, HEIGHT_OFFSET };
    }

    void LevelFileLoader::parseLayers(Context & context, Json & jsonWholeFile)
    {
        context.level.tiles.layers.clear();

        for (Json & jsonLayer : jsonWholeFile["layers"])
        {
            const std::string LAYER_NAME = jsonLayer["name"];

            if (LAYER_NAME == "ground")
            {
                parseTileLayer(context, TileImage::Ground, jsonLayer);
            }
            else if (LAYER_NAME == "object-1")
            {
                parseTileLayer(context, TileImage::Object1, jsonLayer);
            }
            else if (LAYER_NAME == "object-2")
            {
                parseTileLayer(context, TileImage::Object2, jsonLayer);
            }
            else if (LAYER_NAME == "collision")
            {
                parseRectLayer(context, jsonLayer, context.level.walk_collisions);
            }
            else if (LAYER_NAME == "kill")
            {
                parseRectLayer(context, jsonLayer, context.level.kill_collisions);
            }
            else if (LAYER_NAME == "spawn")
            {
                parseSpawnLayer(context, jsonLayer);
            }
            else if (LAYER_NAME == "coin")
            {
                parseCoinLayer(context, jsonLayer);
            }
            else
            {
                M_LOG(
                    "WARNING:  While parsing level file \""
                    << m_path << "\".  Ignored unknown layer named \"" << LAYER_NAME << "\".");
            }
        }

        M_CHECK(
            !context.level.tiles.layers.empty(),
            "Error Parsing Level File " << m_path << ":  Failed to read any tile image layers.");
    }

    void LevelFileLoader::parseTileLayer(Context & context, const TileImage IMAGE, Json & json)
    {
        TileLayer layer;

        layer.image = IMAGE;

        const std::vector<int> INDEXES = json["data"];
        layer.indexes = INDEXES;

        M_CHECK(
            !layer.indexes.empty(),
            "Error Parsing Level File "
                << m_path << ":  Failed to read tileset layer indexes for image " << IMAGE << ".");

        context.level.tiles.layers.push_back(layer);
    }

    void LevelFileLoader::parseRectLayer(
        Context & context, Json & json, std::vector<sf::FloatRect> & rects)
    {
        rects.clear();

        for (Json & collJson : json["objects"])
        {
            rects.emplace_back(parseAndConvertRect(context, collJson));
        }
    }

    sf::FloatRect LevelFileLoader::parseAndConvertRect(const Context & CONTEXT, Json & json)
    {
        sf::IntRect mapRect;
        mapRect.left = json["x"];
        mapRect.top = json["y"];
        mapRect.width = json["width"];
        mapRect.height = json["height"];

        // convert from map to screen coordinates
        sf::FloatRect screenRect{ mapRect };
        screenRect.left *= CONTEXT.settings.tile_scale;
        screenRect.top *= CONTEXT.settings.tile_scale;
        screenRect.width *= CONTEXT.settings.tile_scale;
        screenRect.height *= CONTEXT.settings.tile_scale;
        //
        screenRect.left += CONTEXT.level.map_position_offset.x;
        screenRect.top += CONTEXT.level.map_position_offset.y;

        return screenRect;
    }

    void LevelFileLoader::parseSpawnLayer(Context & context, Json & json)
    {
        context.level.enter_rect = { 0.0f, 0.0f, -1.0f, -1.0f };
        context.level.exit_rect = { 0.0f, 0.0f, -1.0f, -1.0f };

        for (Json & spawnJson : json["objects"])
        {
            const sf::FloatRect RECT = parseAndConvertRect(context, spawnJson);
            const std::string NAME = spawnJson["name"];

            if (NAME == "enter")
            {
                context.level.enter_rect = RECT;
            }

            if (NAME == "exit")
            {
                context.level.exit_rect = RECT;
            }
        }

        M_CHECK(
            (context.level.enter_rect.width > 0.0f),
            "Error Parsing Level File " << m_path << ":  Failed to find enter location.");

        M_CHECK(
            (context.level.exit_rect.width > 0.0f),
            "Error Parsing Level File " << m_path << ":  Failed to find exit location.");
    }

    void LevelFileLoader::parseCoinLayer(Context & context, Json & json)
    {
        context.coins.clear();

        for (Json & coinJson : json["objects"])
        {
            const sf::FloatRect COIN_RECT = parseAndConvertRect(context, coinJson);
            context.coins.add(util::center(COIN_RECT));
        }
    }

} // namespace slnghn
