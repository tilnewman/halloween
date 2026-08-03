#ifndef SPIDER_TEXTURES_HPP_INCLUDED
#define SPIDER_TEXTURES_HPP_INCLUDED
//
// spider-texture.hpp
//
#include <SFML/Graphics/Texture.hpp>

#include <vector>

namespace halloween
{

    struct Context;

    //

    enum class SpiderType : unsigned char
    {
        Mom = 0,
        Dad,
        Child,
        Count
    };

    [[nodiscard]] constexpr std::string_view toString(const SpiderType t_type) noexcept
    {
        // clang-format off
        switch(t_type)
        {
            case SpiderType::Mom:   { return "mom";   }
            case SpiderType::Dad:   { return "dad";   }
            case SpiderType::Child: { return "child"; }
            case SpiderType::Count:    
            default:   { return "unknown_spidertype"; }
        }
        // clang-format on
    }

    //

    enum class SpiderAnim : unsigned char
    {
        Idle = 0,
        Move,
        Attack,
        Death,
        Count
    };

    [[nodiscard]] constexpr std::string_view toString(const SpiderAnim t_anim) noexcept
    {
        // clang-format off
        switch(t_anim)
        {
            case SpiderAnim::Idle:   { return "idle";   }
            case SpiderAnim::Move:   { return "move";   }
            case SpiderAnim::Attack: { return "attack"; }
            case SpiderAnim::Death:  { return "death";  }
            case SpiderAnim::Count:    
            default:     { return "unknown_spideranim"; }
        }
        // clang-format on
    }

    //

    class SpiderTextureManager
    {
      public:
        SpiderTextureManager();

        void setup(const Context & t_context);
        void teardown();

        [[nodiscard]] const std::vector<sf::Texture> &
            textures(const SpiderType t_type, const SpiderAnim t_action) const;

        [[nodiscard]] const sf::Texture & webTexture() const { return m_webTexture; }
 
      private:
        sf::Texture m_webTexture;

        // type vector/anim vector/frame vector
        std::vector<std::vector<std::vector<sf::Texture>>> m_texturesVec;
    };

} // namespace halloween

#endif // SPIDER_TEXTURES_HPP_INCLUDED
