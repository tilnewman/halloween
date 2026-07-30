#ifndef LEVEL_FILE_LOADER_HPP_INCLUDED
#define LEVEL_FILE_LOADER_HPP_INCLUDED
//
// level-file-loader.hpp
//
#include "json.hpp"
#include "tileset.hpp"

#include <string>
#include <vector>

#include <SFML/Graphics/Rect.hpp>

namespace halloween
{
    struct Context;
    struct Settings;
    using Json = nlohmann::json;

    //

    class LevelFileLoader
    {
      public:
        LevelFileLoader();

        bool load(const Context & t_context);

      private:
        void parseLevelDetails(const Context & t_context, Json & json) const;
        void parseObjectTextureGIDs(const Context & t_context, Json & json) const;
        void parseBackgroundImageNumber(const Context & t_context, Json & json) const;
        void parseLayers(const Context & t_context, Json & json) const;
        void parseTileLayer(const Context & t_context, const TileImage image, Json & json) const;

        void parseRectLayer(
            const Context & t_context, Json & json, std::vector<sf::FloatRect> & rects) const;

        const sf::FloatRect parseAndConvertRect(const Context & t_context, Json & json) const;
        void parseSpawnLayer(const Context & t_context, Json & json) const;

        template <typename T>
        void parseObjectLayerRects(T & manager, const Context & t_context, Json & json) const
        {
            for (Json & j : json["objects"])
            {
                const std::string details = j["name"];
                const sf::FloatRect rect{ parseAndConvertRect(t_context, j) };
                manager.add(t_context, rect, details);
            }
        }

      private:
        std::string m_pathStr;
    };

} // namespace halloween

#endif // LEVEL_FILE_LOADER_HPP_INCLUDED
