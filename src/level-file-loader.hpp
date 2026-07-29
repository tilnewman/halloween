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
        void parseLevelDetails(const Context & t_context, Json & json);
        void parseObjectTextureGIDs(const Context & t_context, Json & json);
        void parseBackgroundImageNumber(const Context & t_context, Json & json);
        void parseLayers(const Context & t_context, Json & json);
        void parseTileLayer(const Context & t_context, const TileImage image, Json & json);
        
        void parseRectLayer(
            const Context & t_context, Json & json, std::vector<sf::FloatRect> & rects);
        
        const sf::FloatRect parseAndConvertRect(const Context & t_context, Json & json);
        void parseSpawnLayer(const Context & t_context, Json & json);

        template <typename T>
        void parseObjectLayerRects(T & manager, const Context & t_context, Json & json)
        {
            manager.clear();

            for (Json & j : json["objects"])
            {
                manager.add(t_context, parseAndConvertRect(t_context, j));
            }
        }

      private:
        std::string m_pathStr;
    };

} // namespace halloween

#endif // LEVEL_FILE_LOADER_HPP_INCLUDED
