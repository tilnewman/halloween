#ifndef FROG_TEXTURES_HPP_INCLUDED
#define FROG_TEXTURES_HPP_INCLUDED
//
// frog-texture.hpp
//
#include <SFML/Graphics/Texture.hpp>

#include <vector>
namespace halloween
{

    struct Context;

    //

    enum class FrogAnim : unsigned char
    {
        Idle = 0,
        Hop,
        AttackTounge,
        AttackBite,
        Death,
        Hit,
        Dizzy,
        Eating,
        Roar,
        Count
    };

    [[nodiscard]] constexpr std::string_view toString(const FrogAnim t_anim) noexcept
    {
        // clang-format off
        switch(t_anim)
        {
            case FrogAnim::Idle:         { return "idle";          }
            case FrogAnim::Hop:          { return "hop";           }
            case FrogAnim::AttackTounge: { return "attack-tounge"; }
            case FrogAnim::AttackBite:   { return "attack-bite";   }
            case FrogAnim::Death:        { return "death";         }
            case FrogAnim::Hit:          { return "hit";           }
            case FrogAnim::Dizzy:        { return "dizzy";         }
            case FrogAnim::Eating:       { return "eating";        }
            case FrogAnim::Roar:         { return "roar";          }
            case FrogAnim::Count:    
            default:                  { return "unknown_froganim"; }
        }
        // clang-format on
    }

    //

    class FrogTextureManager
    {
      public:
        FrogTextureManager();

        void setup(const Context & t_context);
        void teardown();

        [[nodiscard]] const std::vector<sf::Texture> & textures(const FrogAnim t_action) const;

      private:
        std::vector<std::vector<sf::Texture>> m_texturesVec;
    };

} // namespace halloween

#endif // FROG_TEXTURES_HPP_INCLUDED
