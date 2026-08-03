// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "level-file-loader.hpp"

#include "bat.hpp"
#include "check-macros.hpp"
#include "coin.hpp"
#include "context.hpp"
#include "dart.hpp"
#include "fire-spout-large.hpp"
#include "fire-spout.hpp"
#include "frogs.hpp"
#include "ghost.hpp"
#include "level.hpp"
#include "moving-platforms.hpp"
#include "mushroom-boss.hpp"
#include "saw.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "slime.hpp"
#include "smoke.hpp"
#include "spiders.hpp"
#include "spiked-ball.hpp"
#include "zombies.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace halloween
{

    LevelFileLoader::LevelFileLoader()
        : m_pathStr()
    {}

    bool LevelFileLoader::load(const Context & t_context)
    {
        std::stringstream fileNameSS;
        fileNameSS << "level-" << t_context.level.number() << ".json";

        const std::filesystem::path path{ (
            t_context.settings.media_path / "map" / fileNameSS.str()) };

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

        parseLevelDetails(t_context, json);
        parseObjectTextureGIDs(t_context, json);
        parseBackgroundImageNumber(t_context, json);

        // everything else in the level file is saved in "layers"
        // which are parsed in order from back to front here, one at a time
        parseLayers(t_context, json);

        return true;
    }

    void LevelFileLoader::parseLevelDetails(const Context & t_context, Json & t_json) const
    {
        // parse level tile size and counts
        const sf::Vector2i tileCount{ t_json["width"], t_json["height"] };
        const sf::Vector2i tileSizeOnMap{ t_json["tilewidth"], t_json["tileheight"] };

        sf::Vector2f tileSizeOnScreen{ sf::Vector2f{ tileSizeOnMap } *
                                       t_context.settings.tile_scale };

        tileSizeOnScreen.x = floorf(tileSizeOnScreen.x);
        tileSizeOnScreen.y = floorf(tileSizeOnScreen.y);

        // calc map position so that the bottom starts at the bottom of the screen
        const sf::Vector2f tileCountF{ tileCount };
        const sf::Vector2f mapSizeOrig{ tileSizeOnScreen * tileCountF };

        const float heightOffset{ (t_context.layout.mapRegion().position.y +
                                   t_context.layout.mapRegion().size.y) -
                                  mapSizeOrig.y };

        const sf::Vector2f mapPosition{ 0.0f, heightOffset };

        t_context.level.setLevelDetails(tileCount, tileSizeOnMap, tileSizeOnScreen, mapPosition);
    }

    void
        LevelFileLoader::parseObjectTextureGIDs(const Context & t_context, Json & t_wholeJson) const
    {
        const auto makeTilesetFilenameTsx = [&](const TileImage t_image) {
            return std::string{ toString(t_image) }.append(".tsx");
        };

        const auto makeTilesetFilenameTsj = [&](const TileImage t_image) {
            return std::string{ toString(t_image) }.append(".tsj");
        };

        for (Json & json : t_wholeJson["tilesets"])
        {
            const std::string sourceStr{ json["source"] };
            const std::filesystem::path path{ sourceStr };
            const std::string filename{ path.filename().string() };

            const int gid{ json["firstgid"] };

            if (filename == makeTilesetFilenameTsx(TileImage::Ground))
            {
                t_context.level.setMapTextureGid(TileImage::Ground, gid);
            }
            else if (filename == makeTilesetFilenameTsx(TileImage::Object1))
            {
                t_context.level.setMapTextureGid(TileImage::Object1, gid);
            }
            else if (filename == makeTilesetFilenameTsx(TileImage::Object2))
            {
                t_context.level.setMapTextureGid(TileImage::Object2, gid);
            }
            else if (filename == makeTilesetFilenameTsx(TileImage::Object3))
            {
                t_context.level.setMapTextureGid(TileImage::Object3, gid);
            }
            else if (filename == makeTilesetFilenameTsj(TileImage::JungleTrees))
            {
                t_context.level.setMapTextureGid(TileImage::JungleTrees, gid);
            }
            else if (filename == makeTilesetFilenameTsj(TileImage::JungleTreesFlip))
            {
                t_context.level.setMapTextureGid(TileImage::JungleTreesFlip, gid);
            }
            else if (filename == makeTilesetFilenameTsj(TileImage::JungleMisc))
            {
                t_context.level.setMapTextureGid(TileImage::JungleMisc, gid);
            }
            else if (filename == makeTilesetFilenameTsj(TileImage::PineTreesDead))
            {
                t_context.level.setMapTextureGid(TileImage::PineTreesDead, gid);
            }
            else
            {
                std::cout << "Warning: While parsing \"" << m_pathStr
                          << "\": Ignored tileset named \"" << filename << "\", with gid=" << gid
                          << "\n";
            }
        }
    }

    void LevelFileLoader::parseBackgroundImageNumber(const Context & t_context, Json & t_json) const
    {
        int backgroundImageNumber = 0;
        for (Json & propJson : t_json["properties"])
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
                         "\"background\", so the default background image 1 will be used.\n";

            backgroundImageNumber = 1;
        }

        t_context.level.backgroundImage(backgroundImageNumber);
    }

    void LevelFileLoader::parseLayers(const Context & t_context, Json & t_jsonWholeFile) const
    {
        for (Json & jsonLayer : t_jsonWholeFile["layers"])
        {
            const std::string layerName = jsonLayer["name"];

            if (layerName == toString(TileImage::Ground))
            {
                parseTileLayer(t_context, TileImage::Ground, jsonLayer);
            }
            else if (layerName == toString(TileImage::Object1))
            {
                parseTileLayer(t_context, TileImage::Object1, jsonLayer);
            }
            else if (layerName == toString(TileImage::Object2))
            {
                parseTileLayer(t_context, TileImage::Object2, jsonLayer);
            }
            else if (layerName == toString(TileImage::Object3))
            {
                parseTileLayer(t_context, TileImage::Object3, jsonLayer);
            }
            else if (layerName == toString(TileImage::JungleTrees))
            {
                parseTileLayer(t_context, TileImage::JungleTrees, jsonLayer);
            }
            else if (layerName == toString(TileImage::JungleTreesFlip))
            {
                parseTileLayer(t_context, TileImage::JungleTreesFlip, jsonLayer);
            }
            else if (layerName == toString(TileImage::JungleMisc))
            {
                parseTileLayer(t_context, TileImage::JungleMisc, jsonLayer);
            }
            else if (layerName == toString(TileImage::PineTreesDead))
            {
                parseTileLayer(t_context, TileImage::PineTreesDead, jsonLayer);
            }
            else if (layerName == "collision")
            {
                std::vector<sf::FloatRect> rects;
                rects.reserve(100);
                parseRectLayer(t_context, jsonLayer, rects);
                t_context.level.setWalkCollisions(rects);
            }
            else if (layerName == "kill")
            {
                std::vector<sf::FloatRect> rects;
                rects.reserve(32);
                parseRectLayer(t_context, jsonLayer, rects);
                t_context.level.setKillCollisions(rects);
            }
            else if (layerName == "acid")
            {
                std::vector<sf::FloatRect> rects;
                rects.reserve(32);
                parseRectLayer(t_context, jsonLayer, rects);
                t_context.level.setAcidCollisions(rects);
            }
            else if (layerName == "water")
            {
                std::vector<sf::FloatRect> rects;
                rects.reserve(32);
                parseRectLayer(t_context, jsonLayer, rects);
                t_context.level.setWaterCollisions(rects);
            }
            else if (layerName == "spawn")
            {
                parseSpawnLayer(t_context, jsonLayer);
            }
            else if (layerName == "moving-platform")
            {
                parseObjectLayerRects(t_context.platforms, t_context, jsonLayer);
            }
            else if (layerName == "coin")
            {
                parseObjectLayerRects(t_context.coins, t_context, jsonLayer);
            }
            else if (layerName == "ghost")
            {
                parseObjectLayerRects(t_context.ghosts, t_context, jsonLayer);
            }
            else if (layerName == "dart")
            {
                parseObjectLayerRects(t_context.darts, t_context, jsonLayer);
            }
            else if (layerName == "spiked-ball")
            {
                parseObjectLayerRects(t_context.balls, t_context, jsonLayer);
            }
            else if (layerName == "fire-spout")
            {
                parseObjectLayerRects(t_context.spouts, t_context, jsonLayer);
            }
            else if (layerName == "fire-spout2")
            {
                parseObjectLayerRects(t_context.spouts_large, t_context, jsonLayer);
            }
            else if (layerName == "saw")
            {
                parseObjectLayerRects(t_context.saws, t_context, jsonLayer);
            }
            else if (layerName == "enemy")
            {
                parseEnemyLayerRects(t_context, jsonLayer);
            }
            else if (layerName == "smoke")
            {
                parseObjectLayerRects(t_context.smoke, t_context, jsonLayer);
            }
            else
            {
                std::cout << "WARNING:  While parsing level file \"" << m_pathStr
                          << "\".  Ignored unknown layer named \"" << layerName << "\".\n";
            }
        }
    }

    void LevelFileLoader::parseEnemyLayerRects(const Context & t_context, Json & t_json) const
    {
        for (Json & objJson : t_json["objects"])
        {
            const std::string name = objJson["name"];
            const sf::FloatRect rect{ parseAndConvertRect(t_context, objJson) };

            if ("slime" == name)
            {
                t_context.slimes.add(t_context, rect, name);
            }
            else if ("bat" == name)
            {
                t_context.bats.add(t_context, rect, name);
            }
            else if ("mushroom" == name)
            {
                t_context.boss.add(t_context, rect, name);
            }
            else if ("zombie" == name)
            {
                t_context.zombies.add(t_context, rect, name);
            }
            else if ("frog" == name)
            {
                t_context.frogs.add(t_context, rect, name);
            }
            else if ("spider" == name)
            {
                t_context.spiders.add(t_context, rect, name);
            }
            else
            {
                std::cout << "WARNING:  While parsing level file \"" << m_pathStr
                          << "\".  Ignored unknown enemy rect named \"" << name << "\".\n";
            }
        }
    }

    void LevelFileLoader::parseTileLayer(
        const Context & t_context, const TileImage image, Json & t_json) const
    {
        TileLayer layer;

        layer.image = image;

        layer.indexes.clear();
        for (const int index : t_json["data"])
        {
            layer.indexes.push_back(index);
        }

        M_CHECK(
            !layer.indexes.empty(),
            "Error Parsing Level File " << m_pathStr
                                        << ":  Failed to read tileset layer indexes for image "
                                        << toString(image) << ".");

        t_context.level.appendTileLayer(layer);
    }

    void LevelFileLoader::parseRectLayer(
        const Context & t_context, Json & t_json, std::vector<sf::FloatRect> & t_rects) const
    {
        t_rects.clear();

        for (Json & collJson : t_json["objects"])
        {
            t_rects.emplace_back(parseAndConvertRect(t_context, collJson));
        }
    }

    const sf::FloatRect
        LevelFileLoader::parseAndConvertRect(const Context & t_context, Json & t_json) const
    {
        const sf::IntRect mapRect{ { t_json["x"], t_json["y"] },
                                   { t_json["width"], t_json["height"] } };

        // convert from map to screen coordinates
        sf::FloatRect screenRect{ mapRect };
        screenRect.position.x *= t_context.settings.tile_scale;
        screenRect.position.y *= t_context.settings.tile_scale;
        screenRect.size.x *= t_context.settings.tile_scale;
        screenRect.size.y *= t_context.settings.tile_scale;
        //
        screenRect.position += t_context.level.mapPosition();

        return screenRect;
    }

    void LevelFileLoader::parseSpawnLayer(const Context & t_context, Json & t_json) const
    {
        sf::FloatRect enterRect;
        sf::FloatRect exitRect;

        for (Json & spawnJson : t_json["objects"])
        {
            const std::string name{ spawnJson["name"] };
            const sf::FloatRect rect{ parseAndConvertRect(t_context, spawnJson) };

            if (name == "enter")
            {
                enterRect = rect;
            }
            else if (name == "exit")
            {
                exitRect = rect;
            }
            else
            {
                std::cout << "WARNING:  While parsing level file \"" << m_pathStr
                          << "\".  Ignored unknown spawn rect named \"" << name << "\".\n";
            }
        }

        M_CHECK(
            (enterRect.size.x > 0.0f),
            "Error Parsing Level File " << m_pathStr << ":  Failed to find enter location.");

        M_CHECK(
            (exitRect.size.x > 0.0f),
            "Error Parsing Level File " << m_pathStr << ":  Failed to find exit location.");

        t_context.level.setEnterAndExitRects(enterRect, exitRect);
    }

} // namespace halloween
