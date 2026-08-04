#ifndef FLY_TEXTURES_HPP_INCLUDED
#define FLY_TEXTURES_HPP_INCLUDED
//
// fly-texture.hpp
//
#include <SFML/Graphics/Texture.hpp>

#include <vector>

namespace halloween
{

    struct Context;

    //

    enum class FlyType : unsigned char
    {
        Beholder = 0,
        Chomp,
        Face,
        Horn,
        Peek,
        Count
    };

    [[nodiscard]] constexpr std::string_view toString(const FlyType t_type) noexcept
    {
        // clang-format off
        switch(t_type)
        {
            case FlyType::Beholder: { return "beholder"; }
            case FlyType::Chomp:    { return "chomp";    }
            case FlyType::Face:     { return "face";     }
            case FlyType::Horn:     { return "horn";     }
            case FlyType::Peek:     { return "Peek";     }
            case FlyType::Count:    
            default:         { return "unknown_flytype"; }
        }
        // clang-format on
    }

    //

    enum class FlyAnim : unsigned char
    {
        Fly = 0,
        Hit,
        Death,
        Count
    };

    [[nodiscard]] constexpr std::string_view toString(const FlyAnim t_anim) noexcept
    {
        // clang-format off
        switch(t_anim)
        {
            case FlyAnim::Fly:   { return "fly";   }
            case FlyAnim::Hit:   { return "hit";   }
            case FlyAnim::Death: { return "death"; }
            case FlyAnim::Count:    
            default:   { return "unknown_flyanim"; }
        }
        // clang-format on
    }

    //

    class FlyTextureManager
    {
      public:
        FlyTextureManager();

        void setup(const Context & t_context);
        void teardown();

        [[nodiscard]] const std::vector<sf::Texture> &
            textures(const FlyType t_type, const FlyAnim t_action) const;

      private:
        // type_vector/anim_vector/frame_vector
        std::vector<std::vector<std::vector<sf::Texture>>> m_texturesVec;
    };

} // namespace halloween

#endif // FLY_TEXTURES_HPP_INCLUDED
