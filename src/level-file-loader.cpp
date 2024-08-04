// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "level-file-loader.hpp"

#include "check-macros.hpp"
#include "coin.hpp"
#include "context.hpp"
#include "dart.hpp"
#include "ghost.hpp"
#include "level.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "slime.hpp"
#include "spiked-ball.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace halloween
{

    LevelFileLoader::LevelFileLoader()
        : m_pathStr()
    {}

    bool LevelFileLoader::load(Context & context)
    {
        std::stringstream fileNameSS;
        fileNameSS << "level-" << context.level_number << ".json";

        const std::filesystem::path path = (context.settings.media_path / "map" / fileNameSS.str());
        if (!std::filesystem::exists(path))
        {
            return false;
        }

        m_pathStr = path.string();

        // open file
        std::ifstream iStream(m_pathStr);
        M_CHECK(iStream, "Failed to open level file: \"" << m_pathStr << "\"");

        // stream file into json parser
        Json json;
        iStream >> json;

        parseLevelDetails(context, json);

        // everything else in the level file is saved in "layers"
        // which are parsed in order from back to front here, one at a time
        parseLayers(context, json);

        return true;
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
        const sf::Vector2f tileCountF{ context.level.tiles.count };
        const sf::Vector2f mapSizeOrig{ context.level.tile_size_screen * tileCountF };

        const float heightOffset{
            (context.layout.mapRegion().top + context.layout.mapRegion().height) - mapSizeOrig.y
        };

        context.level.map_position_offset = { 0.0f, heightOffset };
    }

    void LevelFileLoader::parseLayers(Context & context, Json & jsonWholeFile)
    {
        context.level.tiles.layers.clear();

        for (Json & jsonLayer : jsonWholeFile["layers"])
        {
            const std::string layerName = jsonLayer["name"];

            if (layerName == "ground")
            {
                parseTileLayer(context, TileImage::Ground, jsonLayer);
            }
            else if (layerName == "object-1")
            {
                parseTileLayer(context, TileImage::Object1, jsonLayer);
            }
            else if (layerName == "object-2")
            {
                parseTileLayer(context, TileImage::Object2, jsonLayer);
            }
            else if (layerName == "collision")
            {
                parseRectLayer(context, jsonLayer, context.level.walk_collisions);
            }
            else if (layerName == "kill")
            {
                parseRectLayer(context, jsonLayer, context.level.kill_collisions);
            }
            else if (layerName == "acid")
            {
                parseRectLayer(context, jsonLayer, context.level.acid_collisions);
            }
            else if (layerName == "water")
            {
                parseRectLayer(context, jsonLayer, context.level.water_collisions);
            }
            else if (layerName == "spawn")
            {
                parseSpawnLayer(context, jsonLayer);
            }
            else if (layerName == "coin")
            {
                parseCoinLayer(context, jsonLayer);
            }
            else if (layerName == "ghost")
            {
                parseGhostLayer(context, jsonLayer);
            }
            else if (layerName == "slime")
            {
                parseSlimeLayer(context, jsonLayer);
            }
            else if (layerName == "dart")
            {
                parseDartLayer(context, jsonLayer);
            }
            else if (layerName == "spiked-ball")
            {
                parseSpikedBallLayer(context, jsonLayer);
            }
            else
            {
                M_LOG(
                    "WARNING:  While parsing level file \""
                    << m_pathStr << "\".  Ignored unknown layer named \"" << layerName << "\".");
            }
        }

        M_CHECK(
            !context.level.tiles.layers.empty(),
            "Error Parsing Level File " << m_pathStr << ":  Failed to read any tile image layers.");
    }

    void LevelFileLoader::parseTileLayer(Context & context, const TileImage image, Json & json)
    {
        TileLayer layer;

        layer.image = image;

        const std::vector<int> indexes = json["data"];
        layer.indexes = indexes;

        M_CHECK(
            !layer.indexes.empty(),
            "Error Parsing Level File "
                << m_pathStr << ":  Failed to read tileset layer indexes for image " << image
                << ".");

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

    const sf::FloatRect LevelFileLoader::parseAndConvertRect(const Context & context, Json & json)
    {
        sf::IntRect mapRect;
        mapRect.left = json["x"];
        mapRect.top = json["y"];
        mapRect.width = json["width"];
        mapRect.height = json["height"];

        // convert from map to screen coordinates
        sf::FloatRect screenRect{ mapRect };
        screenRect.left *= context.settings.tile_scale;
        screenRect.top *= context.settings.tile_scale;
        screenRect.width *= context.settings.tile_scale;
        screenRect.height *= context.settings.tile_scale;
        //
        screenRect.left += context.level.map_position_offset.x;
        screenRect.top += context.level.map_position_offset.y;

        return screenRect;
    }

    void LevelFileLoader::parseSpawnLayer(Context & context, Json & json)
    {
        context.level.enter_rect = { 0.0f, 0.0f, -1.0f, -1.0f };
        context.level.exit_rect = { 0.0f, 0.0f, -1.0f, -1.0f };

        for (Json & spawnJson : json["objects"])
        {
            const std::string name = spawnJson["name"];
            const sf::FloatRect rect = parseAndConvertRect(context, spawnJson);

            if (name == "enter")
            {
                context.level.enter_rect = rect;
            }
            else if (name == "exit")
            {
                context.level.exit_rect = rect;
            }
        }

        M_CHECK(
            (context.level.enter_rect.width > 0.0f),
            "Error Parsing Level File " << m_pathStr << ":  Failed to find enter location.");

        M_CHECK(
            (context.level.exit_rect.width > 0.0f),
            "Error Parsing Level File " << m_pathStr << ":  Failed to find exit location.");
    }

    void LevelFileLoader::parseCoinLayer(Context & context, Json & json)
    {
        context.coins.clear();

        for (Json & coinJson : json["objects"])
        {
            const sf::FloatRect rect = parseAndConvertRect(context, coinJson);
            context.coins.add(util::center(rect));
        }
    }

    void LevelFileLoader::parseGhostLayer(Context & context, Json & json)
    {
        context.ghosts.clearSpawnPoints();

        for (Json & ghostJson : json["objects"])
        {
            const sf::FloatRect rect = parseAndConvertRect(context, ghostJson);
            context.ghosts.addSpawnPoint(context, util::center(rect));
        }
    }

    void LevelFileLoader::parseSlimeLayer(Context & context, Json & json)
    {
        context.slimes.clearRects();

        for (Json & slimeJson : json["objects"])
        {
            const sf::FloatRect rect = parseAndConvertRect(context, slimeJson);
            context.slimes.addRect(rect);
        }
    }

    void LevelFileLoader::parseDartLayer(Context & context, Json & json)
    {
        context.darts.clear();

        for (Json & dartJson : json["objects"])
        {
            const sf::FloatRect rect = parseAndConvertRect(context, dartJson);
            context.darts.add(util::center(rect));
        }
    }

    void LevelFileLoader::parseSpikedBallLayer(Context & context, Json & json)
    {
        context.balls.clear();

        for (Json & ballJson : json["objects"])
        {
            const sf::FloatRect rect = parseAndConvertRect(context, ballJson);
            context.balls.add(rect);
        }
    }

} // namespace halloween
