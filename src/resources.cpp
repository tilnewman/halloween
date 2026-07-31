// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// resources.cpp
//
#include "resources.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "screen-regions.hpp"
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
        , m_tileJungleTrees{}
        , m_tileJungleTreesFlip{}
        , m_tileJungleMisc{}
        , m_bgTexture1{}
        , m_bgTexture2{}
        , m_bgTexture3{}
        , m_bgSprite{ m_bgTexture1 }
    {}

    void Resources::setup(const Settings & t_settings)
    {
        // font
        const std::string fontPathStr{
            (t_settings.media_path / "font" / "mops-antiqua.ttf").string()
        };

        const bool fontLoadSuccess{ m_font.openFromFile(fontPathStr) };
        M_CHECK(fontLoadSuccess, "Failed to load font: " << fontPathStr);

        // background image
        util::TextureLoader::load(
            m_bgTexture1, (t_settings.media_path / "image" / "background-1.png"), true);

        util::TextureLoader::load(
            m_bgTexture2, (t_settings.media_path / "image" / "background-2.png"), true);

        util::TextureLoader::load(
            m_bgTexture3, (t_settings.media_path / "image" / "background-3.png"), true);

        m_bgSprite.setTexture(m_bgTexture1, true);

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

        util::TextureLoader::load(m_tileJungleTrees.texture, (imagePath + "tile-jungle-trees.png"));
        m_tileJungleTrees.which = TileImage::JungleTrees;
        m_tileJungleTrees.size = sf::Vector2i(m_tileJungleTrees.texture.getSize());

        util::TextureLoader::load(m_tileJungleMisc.texture, (imagePath + "tile-jungle-misc.png"));
        m_tileJungleMisc.which = TileImage::JungleMisc;
        m_tileJungleMisc.size = sf::Vector2i(m_tileJungleMisc.texture.getSize());

        util::TextureLoader::load(
            m_tileJungleTreesFlip.texture, (imagePath + "tile-jungle-trees-flip.png"));

        m_tileJungleTreesFlip.which = TileImage::JungleTreesFlip;
        m_tileJungleTreesFlip.size = sf::Vector2i(m_tileJungleTreesFlip.texture.getSize());
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
            case TileImage::Ground:          { return m_groundTexture;       }
            case TileImage::Object1:         { return m_objectTexture1;      }
            case TileImage::Object2:         { return m_objectTexture2;      }
            case TileImage::Object3:         { return m_objectTexture3;      }
            case TileImage::JungleTrees:     { return m_tileJungleTrees;     }
            case TileImage::JungleTreesFlip: { return m_tileJungleTreesFlip; }
            case TileImage::JungleMisc:      { return m_tileJungleMisc;      }
            default:
            {
                throw std::runtime_error("Resources::tileTexture() given an invalid TileImage enum.");
            }
        }
        // clang-format on
    }

    void Resources::setupBackgroundSprite(const Context & t_context, const int t_bgImageNumber)
    {
        if (1 == t_bgImageNumber)
        {
            m_bgSprite.setTexture(m_bgTexture1);
        }
        else if (2 == t_bgImageNumber)
        {
            m_bgSprite.setTexture(m_bgTexture2);
        }
        else if (3 == t_bgImageNumber)
        {
            m_bgSprite.setTexture(m_bgTexture3);
        }
        else
        {
            M_LOG(
                "Error: This map file has an invalid custom background property value="
                << t_bgImageNumber << ", so the default background image 1 will be used.");

            m_bgSprite.setTexture(m_bgTexture1);
        }

        util::growAndCenterInside(m_bgSprite, t_context.layout.wholeRegion());
    }

} // namespace halloween
