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

    struct Context;
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

    class Resources
    {
      public:
        Resources();

        void setup(const Settings & t_settings);

        const sf::Text makeText(
            const unsigned int t_charSize,
            const std::string & t_message,
            const sf::Color & t_color) const;

        const TileTexture & tileTexture(const TileImage t_image) const;

        constexpr void setGidGround(const int t_gid) { m_groundTexture.gid = t_gid; }
        constexpr void setGidObject1(const int t_gid) { m_objectTexture1.gid = t_gid; }
        constexpr void setGidObject2(const int t_gid) { m_objectTexture2.gid = t_gid; }
        constexpr void setGidObject3(const int t_gid) { m_objectTexture3.gid = t_gid; }
        constexpr void setGidJungleTrees(const int t_gid) { m_tileJungleTrees.gid = t_gid; }
        constexpr void setGidJungleTreesFlip(const int t_gid) { m_tileJungleTreesFlip.gid = t_gid; }
        constexpr void setGidJungleMisc(const int t_gid) { m_tileJungleMisc.gid = t_gid; }

        inline const sf::Sprite backgroundSprite() const { return m_bgSprite; }

        void setupBackgroundSprite(const Context & t_context, const int t_bgImageNumber);

      private:
        sf::Font m_font;
        TileTexture m_groundTexture;
        TileTexture m_objectTexture1;
        TileTexture m_objectTexture2;
        TileTexture m_objectTexture3;
        TileTexture m_tileJungleTrees;
        TileTexture m_tileJungleTreesFlip;
        TileTexture m_tileJungleMisc;
        sf::Texture m_bgTexture1;
        sf::Texture m_bgTexture2;
        sf::Texture m_bgTexture3;
        sf::Sprite m_bgSprite;
    };

} // namespace halloween

#endif // RESOURCES_HPP_INCLUDED
