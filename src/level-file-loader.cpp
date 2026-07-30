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
#include "smoke.hpp"
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

    void LevelFileLoader::parseLevelDetails(const Context & t_context, Json & json) const
    {
        // parse level tile size and counts
        const sf::Vector2i tileCount{ json["width"], json["height"] };
        const sf::Vector2i tileSizeOnMap{ json["tilewidth"], json["tileheight"] };

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

    void LevelFileLoader::parseObjectTextureGIDs(const Context & t_context, Json & wholeJson) const
    {
        for (Json & json : wholeJson["tilesets"])
        {
            const std::string sourceStr{ json["source"] };
            const std::filesystem::path path{ sourceStr };
            const std::string filename{ path.filename().string() };

            const int gid{ json["firstgid"] };

            if (filename == "ground.tsx")
            {
                t_context.media.ground_texture.gid = gid;
            }
            else if (filename == "object-1.tsx")
            {
                t_context.media.object_texture1.gid = gid;
            }
            else if (filename == "object-2.tsx")
            {
                t_context.media.object_texture2.gid = gid;
            }
            else if (filename == "object-3.tsx")
            {
                t_context.media.object_texture3.gid = gid;
            }
            else if (filename == "object-4.tsj")
            {
                t_context.media.object_texture4.gid = gid;
            }
            else
            {
                std::cout << "Warning: While parsing \"" << m_pathStr
                          << "\": Ignored tileset named \"" << filename << "\", with gid=" << gid
                          << "\n";
            }
        }
    }

    void LevelFileLoader::parseBackgroundImageNumber(const Context & t_context, Json & json) const
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
            t_context.media.bg_sprite.setTexture(t_context.media.bg_texture1);
        }
        else if (2 == backgroundImageNumber)
        {
            t_context.media.bg_sprite.setTexture(t_context.media.bg_texture2);
        }
        else if (3 == backgroundImageNumber)
        {
            t_context.media.bg_sprite.setTexture(t_context.media.bg_texture3);
        }
        else
        {
            std::cout << "Error: While parsing \"" << m_pathStr
                      << "\": This map file has an invalid custom background property value="
                      << backgroundImageNumber
                      << ", so the default background image #1 will be used.\n";
        }

        util::growAndCenterInside(t_context.media.bg_sprite, t_context.layout.wholeRegion());
    }

    void LevelFileLoader::parseLayers(const Context & t_context, Json & jsonWholeFile) const
    {
        for (Json & jsonLayer : jsonWholeFile["layers"])
        {
            const std::string layerName = jsonLayer["name"];

            if (layerName == "ground")
            {
                parseTileLayer(t_context, TileImage::Ground, jsonLayer);
            }
            else if (layerName == "object-1")
            {
                parseTileLayer(t_context, TileImage::Object1, jsonLayer);
            }
            else if (layerName == "object-2")
            {
                parseTileLayer(t_context, TileImage::Object2, jsonLayer);
            }
            else if (layerName == "object-3")
            {
                parseTileLayer(t_context, TileImage::Object3, jsonLayer);
            }
            else if (layerName == "object-4")
            {
                parseTileLayer(t_context, TileImage::Object4, jsonLayer);
            }
            else if (layerName == "collision")
            {
                std::vector<sf::FloatRect> rects;
                rects.reserve(1'000);
                parseRectLayer(t_context, jsonLayer, rects);
                t_context.level.setWalkCollisions(rects);
            }
            else if (layerName == "kill")
            {
                std::vector<sf::FloatRect> rects;
                rects.reserve(100);
                parseRectLayer(t_context, jsonLayer, rects);
                t_context.level.setKillCollisions(rects);
            }
            else if (layerName == "acid")
            {
                std::vector<sf::FloatRect> rects;
                rects.reserve(100);
                parseRectLayer(t_context, jsonLayer, rects);
                t_context.level.setAcidCollisions(rects);
            }
            else if (layerName == "water")
            {
                std::vector<sf::FloatRect> rects;
                rects.reserve(100);
                parseRectLayer(t_context, jsonLayer, rects);
                t_context.level.setWaterCollisions(rects);
            }
            else if (layerName == "spawn")
            {
                parseSpawnLayer(t_context, jsonLayer);
            }
            else if (layerName == "coin")
            {
                parseObjectLayerRects(t_context.coins, t_context, jsonLayer);
            }
            else if (layerName == "ghost")
            {
                parseObjectLayerRects(t_context.ghosts, t_context, jsonLayer);
            }
            else if (layerName == "slime")
            {
                parseObjectLayerRects(t_context.slimes, t_context, jsonLayer);
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
            else if (layerName == "saw")
            {
                parseObjectLayerRects(t_context.saws, t_context, jsonLayer);
            }
            else if (layerName == "bat")
            {
                parseObjectLayerRects(t_context.bats, t_context, jsonLayer);
            }
            else if (layerName == "boss")
            {
                parseObjectLayerRects(t_context.boss, t_context, jsonLayer);
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

    void LevelFileLoader::parseTileLayer(
        const Context & t_context, const TileImage image, Json & json) const
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

        t_context.level.appendTileLayer(layer);
    }

    void LevelFileLoader::parseRectLayer(
        const Context & t_context, Json & json, std::vector<sf::FloatRect> & rects) const
    {
        rects.clear();

        for (Json & collJson : json["objects"])
        {
            rects.emplace_back(parseAndConvertRect(t_context, collJson));
        }
    }

    const sf::FloatRect LevelFileLoader::parseAndConvertRect(const Context & t_context, Json & json) const
    {
        const sf::IntRect mapRect{ { json["x"], json["y"] }, { json["width"], json["height"] } };

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

    void LevelFileLoader::parseSpawnLayer(const Context & t_context, Json & json) const
    {
        sf::FloatRect enterRect{ { 0.0f, 0.0f }, { 0.0f, 0.0f } };
        sf::FloatRect exitRect{ { 0.0f, 0.0f }, { 0.0f, 0.0f } };

        for (Json & spawnJson : json["objects"])
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
