// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// resources.cpp
//
#include "resources.hpp"

#include "check-macros.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "texture-loader.hpp"

namespace halloween
{

    Resources::Resources()
        : m_font{}
        , m_groundTexture{}
        , m_objectTexture1{}
        , m_objectTexture2{}
        , m_objectTexture3{}
        , bg_texture1{}
        , bg_texture2{}
        , bg_texture3{}
        , bg_sprite{ bg_texture1 }
    {}

    void Resources::setup(const Settings & t_settings)
    {
        // font
        if (!m_font.openFromFile((t_settings.media_path / "font" / "mops-antiqua.ttf").string()))
        {
            std::cout << "Failed to load font mops-antiqua.ttf!\n";
        }

        // background image
        util::TextureLoader::load(
            bg_texture1, (t_settings.media_path / "image" / "background-1.png"), true);

        util::TextureLoader::load(
            bg_texture2, (t_settings.media_path / "image" / "background-2.png"), true);

        util::TextureLoader::load(
            bg_texture3, (t_settings.media_path / "image" / "background-3.png"), true);

        bg_sprite.setTexture(bg_texture1, true);

        const std::string imagePath{ (t_settings.media_path / "image" / "map/").string() };

        util::TextureLoader::load(m_groundTexture.texture, (imagePath + "tile-ground.png"));
        m_groundTexture.which = TileImage::Ground;
        m_groundTexture.size = sf::Vector2i(m_groundTexture.texture.getSize());

        util::TextureLoader::load(m_objectTexture1.texture, (imagePath + "tile-object-1.png"));
        m_objectTexture1.which = TileImage::Object1;
        m_objectTexture1.size = sf::Vector2i(m_objectTexture1.texture.getSize());

        util::TextureLoader::load(m_objectTexture2.texture, (imagePath + "tile-object-2.png"));
        m_objectTexture2.which = TileImage::Object2;
        m_objectTexture2.size = sf::Vector2i(m_objectTexture2.texture.getSize());

        util::TextureLoader::load(m_objectTexture3.texture, (imagePath + "tile-object-3.png"));
        m_objectTexture3.which = TileImage::Object3;
        m_objectTexture3.size = sf::Vector2i(m_objectTexture3.texture.getSize());

        util::TextureLoader::load(m_objectTexture4.texture, (imagePath + "tile-object-4.png"));
        m_objectTexture4.which = TileImage::Object4;
        m_objectTexture4.size = sf::Vector2i(m_objectTexture4.texture.getSize());

        util::TextureLoader::load(m_objectTexture5.texture, (imagePath + "tile-object-5.png"));
        m_objectTexture5.which = TileImage::Object5;
        m_objectTexture5.size = sf::Vector2i(m_objectTexture5.texture.getSize());
    }

    const sf::Text Resources::makeText(
        const unsigned int t_charSize,
        const std::string & t_message,
        const sf::Color & t_color) const
    {
        sf::Text text(m_font, t_message, t_charSize);
        text.setFillColor(t_color);
        util::setOriginToPosition(text);
        return text;
    }

    const TileTexture & Resources::tileTexture(const TileImage image) const
    {
        // clang-format off
        switch (image)
        {
            case TileImage::Ground:  { return m_groundTexture;  }
            case TileImage::Object1: { return m_objectTexture1; }
            case TileImage::Object2: { return m_objectTexture2; }
            case TileImage::Object3: { return m_objectTexture3; }
            case TileImage::Object4: { return m_objectTexture4; }
            case TileImage::Object5: { return m_objectTexture5; }
            default:
            {
                throw std::runtime_error("Resources::tileTexture() given an invalid TileImage enum.");
            }
        }
        // clang-format on
    }

} // namespace halloween
