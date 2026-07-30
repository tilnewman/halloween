#ifndef RESOURCES_HPP_INCLUDED
#define RESOURCES_HPP_INCLUDED
//
// resources.hpp
//
#include "tileset.hpp"

#include <string>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{
    struct Settings;

    //

    struct TileTexture
    {
        TileTexture()
            : which{ TileImage::Ground } // any default works here
            , size{}
            , texture{}
            , gid{ 0 }
        {}

        TileImage which;
        sf::Vector2i size;
        sf::Texture texture;
        int gid;
    };

    //

    struct Resources
    {
        Resources();

        void setup(const Settings & t_settings);

        const sf::Text makeText(
            const unsigned int t_charSize,
            const std::string & t_message,
            const sf::Color & t_color) const;

        const TileTexture & tileTexture(const TileImage t_image) const;

        sf::Font font;
        sf::Text fps_text;

        sf::Texture bg_texture1;
        sf::Texture bg_texture2;
        sf::Texture bg_texture3;
        sf::Sprite bg_sprite;

        TileTexture ground_texture;
        TileTexture object_texture1;
        TileTexture object_texture2;
        TileTexture object_texture3;
        TileTexture object_texture4;
        TileTexture object_texture5;
    };

} // namespace halloween

#endif // RESOURCES_HPP_INCLUDED
