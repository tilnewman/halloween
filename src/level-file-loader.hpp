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
        void parseLevelDetails(const Context & t_context, Json & t_json) const;
        void parseObjectTextureGIDs(const Context & t_context, Json & t_json) const;
        void parseBackgroundImageNumber(const Context & t_context, Json & t_json) const;
        void parseLayers(const Context & t_context, Json & t_json) const;
        void parseTileLayer(const Context & t_context, const TileImage image, Json & t_json) const;

        void parseRectLayer(
            const Context & t_context, Json & t_json, std::vector<sf::FloatRect> & t_rects) const;

        const sf::FloatRect parseAndConvertRect(const Context & t_context, Json & t_json) const;
        void parseSpawnLayer(const Context & t_context, Json & t_json) const;

        template <typename T>
        void parseObjectLayerRects(T & t_manager, const Context & t_context, Json & t_json) const
        {
            for (Json & objJson : t_json["objects"])
            {
                const std::string details = objJson["name"];
                const sf::FloatRect rect{ parseAndConvertRect(t_context, objJson) };
                t_manager.add(t_context, rect, details);
            }
        }

      private:
        std::string m_pathStr;
    };

} // namespace halloween

#endif // LEVEL_FILE_LOADER_HPP_INCLUDED
