// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "level-file-loader.hpp"

#include "bat.hpp"
#include "check-macros.hpp"
#include "coin.hpp"
#include "context.hpp"
#include "dart.hpp"
#include "fire-spout.hpp"
#include "ghost.hpp"
#include "level.hpp"
#include "mushroom-boss.hpp"
#include "resources.hpp"
#include "saw.hpp"
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

        Json json;

        {
            std::ifstream iStream(m_pathStr);
            M_CHECK(iStream, "Failed to open level file: \"" << m_pathStr << "\"");
            iStream >> json;
        }

        parseLevelDetails(context, json);
        parseObjectTextureGIDs(context, json);
        parseBackgroundImageNumber(context, json);

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

        const float heightOffset{ (context.layout.mapRegion().position.y +
                                   context.layout.mapRegion().size.y) -
                                  mapSizeOrig.y };

        context.level.map_position_offset = { 0.0f, heightOffset };
    }

    void LevelFileLoader::parseObjectTextureGIDs(Context & context, Json & wholeJson)
    {
        for (Json & json : wholeJson["tilesets"])
        {
            const std::string sourceStr{ json["source"] };
            const std::filesystem::path path{ sourceStr };
            const std::string filename{ path.filename().string() };

            const int gid{ json["firstgid"] };

            if (filename == "ground.tsx")
            {
                context.media.ground_texture.gid = gid;
            }
            else if (filename == "object-1.tsx")
            {
                context.media.object_texture1.gid = gid;
            }
            else if (filename == "object-2.tsx")
            {
                context.media.object_texture2.gid = gid;
            }
            else if (filename == "object-3.tsx")
            {
                context.media.object_texture3.gid = gid;
            }
            else
            {
                std::cout << "Warning: While parsing \"" << m_pathStr
                          << "\": Ignored tileset named \"" << filename << "\", with gid=" << gid
                          << "\n";
            }
        }
    }

    void LevelFileLoader::parseBackgroundImageNumber(Context & context, Json & json)
    {
        int backgroundImageNumber = 0;
        for (Json & propJson : json["properties"])
        {
            const std::string propName = propJson["name"];
            if ("background" == propName)
            {
                backgroundImageNumber = propJson["value"];
                break;
            }
            else
            {
                std::cout << "Warning: While parsing \"" << m_pathStr
                          << "\": Ignored custom property named \"" << propName << "\"\n";
            }
        }

        if (0 == backgroundImageNumber)
        {
            std::cout << "Error: While parsing \"" << m_pathStr
                      << "\": This map file is missing the custom int property named "
                         "\"background\", so the default background image #1 will be used.\n";

            backgroundImageNumber = 1;
        }

        if (1 == backgroundImageNumber)
        {
            context.media.bg_sprite.setTexture(context.media.bg_texture1);
        }
        else if (2 == backgroundImageNumber)
        {
            context.media.bg_sprite.setTexture(context.media.bg_texture2);
        }
        else if (3 == backgroundImageNumber)
        {
            context.media.bg_sprite.setTexture(context.media.bg_texture3);
        }
        else
        {
            std::cout << "Error: While parsing \"" << m_pathStr
                      << "\": This map file has an invalid custom background property value="
                      << backgroundImageNumber
                      << ", so the default background image #1 will be used.\n";
        }

        util::growAndCenterInside(context.media.bg_sprite, context.layout.wholeRegion());
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
            else if (layerName == "object-3")
            {
                parseTileLayer(context, TileImage::Object3, jsonLayer);
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
                parseObjectLayerRects(context.coins, context, jsonLayer);
            }
            else if (layerName == "ghost")
            {
                parseObjectLayerRects(context.ghosts, context, jsonLayer);
            }
            else if (layerName == "slime")
            {
                parseObjectLayerRects(context.slimes, context, jsonLayer);
            }
            else if (layerName == "dart")
            {
                parseObjectLayerRects(context.darts, context, jsonLayer);
            }
            else if (layerName == "spiked-ball")
            {
                parseObjectLayerRects(context.balls, context, jsonLayer);
            }
            else if (layerName == "fire-spout")
            {
                parseObjectLayerRects(context.spouts, context, jsonLayer);
            }
            else if (layerName == "saw")
            {
                parseObjectLayerRects(context.saws, context, jsonLayer);
            }
            else if (layerName == "bat")
            {
                parseObjectLayerRects(context.bats, context, jsonLayer);
            }
            else if (layerName == "boss")
            {
                parseObjectLayerRects(context.boss, context, jsonLayer);
            }
            else
            {
                std::cout << "WARNING:  While parsing level file \"" << m_pathStr
                          << "\".  Ignored unknown layer named \"" << layerName << "\".\n";
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
        mapRect.position.x = json["x"];
        mapRect.position.y = json["y"];
        mapRect.size.x = json["width"];
        mapRect.size.y = json["height"];

        // convert from map to screen coordinates
        sf::FloatRect screenRect{ mapRect };
        screenRect.position.x *= context.settings.tile_scale;
        screenRect.position.y *= context.settings.tile_scale;
        screenRect.size.x *= context.settings.tile_scale;
        screenRect.size.y *= context.settings.tile_scale;
        //
        screenRect.position.x += context.level.map_position_offset.x;
        screenRect.position.y += context.level.map_position_offset.y;

        return screenRect;
    }

    void LevelFileLoader::parseSpawnLayer(Context & context, Json & json)
    {
        context.level.enter_rect = { { 0.0f, 0.0f }, { -1.0f, -1.0f } };
        context.level.exit_rect = { { 0.0f, 0.0f }, { -1.0f, -1.0f } };

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
            else
            {
                std::cout << "WARNING:  While parsing level file \"" << m_pathStr
                          << "\".  Ignored unknown spawn rect named \"" << name << "\".\n";
            }
        }

        M_CHECK(
            (context.level.enter_rect.size.x > 0.0f),
            "Error Parsing Level File " << m_pathStr << ":  Failed to find enter location.");

        M_CHECK(
            (context.level.exit_rect.size.x > 0.0f),
            "Error Parsing Level File " << m_pathStr << ":  Failed to find exit location.");
    }

} // namespace halloween
