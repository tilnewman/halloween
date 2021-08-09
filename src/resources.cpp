// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// resources.cpp
//
#include "resources.hpp"

#include "check-macros.hpp"
#include "settings.hpp"
#include "util.hpp"

namespace halloween
{

    Resources::Resources()
        : will_smooth_textures(true)
        , font()
        , fps_text("", font, 60)
        , bg_texture()
        , bg_sprite()
        , ground_texture()
        , object_texture1()
        , object_texture2()
    {}

    void Resources::setup(const Settings & settings)
    {
        // creepy font
        font.loadFromFile((settings.media_path / "font/mops-antiqua.ttf").string());
        fps_text.setFont(font);

        const std::string IMAGE_PATH = (settings.media_path / "image/map/").string();

        // background image
        bg_texture.loadFromFile(IMAGE_PATH + "/background.png");
        bg_texture.setSmooth(true);

        bg_sprite.setTexture(bg_texture);

        // ground map tile image
        ground_texture.which = TileImage::Ground;
        ground_texture.texture.loadFromFile(IMAGE_PATH + "/tile-ground.png");
        ground_texture.texture.setSmooth(will_smooth_textures);
        ground_texture.size = sf::Vector2i(ground_texture.texture.getSize());
        ground_texture.gid = 1;

        // object-1 map tile image
        object_texture1.texture.loadFromFile(IMAGE_PATH + "/tile-object-1.png");
        object_texture1.which = TileImage::Object1;
        object_texture1.texture.setSmooth(will_smooth_textures);
        object_texture1.size = sf::Vector2i(object_texture1.texture.getSize());
        object_texture1.gid = 513;

        // object-2 map tile image
        object_texture2.texture.loadFromFile(IMAGE_PATH + "/tile-object-2.png");
        object_texture2.which = TileImage::Object2;
        object_texture2.texture.setSmooth(will_smooth_textures);
        object_texture2.size = sf::Vector2i(object_texture2.texture.getSize());
        object_texture2.gid = 257;
    }

} // namespace halloween
