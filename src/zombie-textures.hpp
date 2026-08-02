#ifndef ZOMBIE_TEXTURES_HPP_INCLUDED
#define ZOMBIE_TEXTURES_HPP_INCLUDED
//
// zombie-texture.hpp
//
#include <SFML/Graphics/Texture.hpp>

#include <vector>
namespace halloween
{

    struct Context;

    enum class ZombieAnim : unsigned char
    {
        Idle = 0,
        Walk,
        Attack,
        Die,
        Hit,
        Count
    };

    [[nodiscard]] constexpr std::string_view toString(const ZombieAnim t_anim) noexcept
    {
        // clang-format off
        switch(t_anim)
        {
            case ZombieAnim::Idle:   { return "idle";   }
            case ZombieAnim::Walk:   { return "walk";   }
            case ZombieAnim::Attack: { return "attack"; }
            case ZombieAnim::Die:    { return "die";    }
            case ZombieAnim::Hit:    { return "hit";    }
            case ZombieAnim::Count:    
            default:     { return "unknown_zombieanim"; }
        }
        // clang-format on
    }

    //

    class ZombieTextureManager
    {
      public:
        ZombieTextureManager();

        void setup(const Context & t_context);
        void teardown();

        [[nodiscard]] const std::vector<sf::Texture> & textures(const ZombieAnim t_action) const;

      private:
        std::vector<std::vector<sf::Texture>> m_texturesVec;
    };

} // namespace halloween

#endif // ZOMBIE_TEXTURES_HPP_INCLUDED
