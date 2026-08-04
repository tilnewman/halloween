#ifndef SPIDER_HPP_INCLUDED
#define SPIDER_HPP_INCLUDED
//
// spider.hpp
//
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
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

    enum class SpiderTask : unsigned char
    {
        Wait,
        Descend,
        Attack,
        Ascend,
        Death
    };

    //

    [[nodiscard]] constexpr float timePerFrameSec(const SpiderAnim t_anim) noexcept
    {
        // clang-format off
        switch(t_anim)
        {
            case SpiderAnim::Move:   { return 0.1f; }
            case SpiderAnim::Attack: { return 0.1f; }
            case SpiderAnim::Death:  { return 0.1f; }
            case SpiderAnim::Idle:  
            case SpiderAnim::Count:
            default:                 { return 0.05f; }
        }
        // clang-format on
    }

    //

    class Spider
    {
      public:
        Spider(
            const Context & t_context,
            const sf::FloatRect & t_rect,
            const sf::Texture & t_webTexture,
            const std::vector<std::vector<std::vector<sf::Texture>>> & t_texturesVecVec);

        [[nodiscard]] const sf::FloatRect collisionRect() const;
        void update(const Context & t_context, const float t_frameTimeSec);
        void moveWithMap(const sf::Vector2f & t_move);

        bool doesAvatarCollideWithAnyAndDie(
            const Context & t_context, const sf::FloatRect & t_avatarRect);

        [[nodiscard]] constexpr bool isAlive() const noexcept { return (m_hitPoints > 0); }
        void hit(const Context & t_context);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

      private:
        void setupTask(const SpiderTask t_task, const SpiderAnim t_anim);
        void changeTextureWithoutMovingSprite(const sf::Texture & t_texture);

        [[nodiscard]] const std::vector<sf::Texture> &
            getTextures(const SpiderType t_type, const SpiderAnim t_action) const;

      private:
        SpiderAnim m_anim;
        SpiderType m_type;
        SpiderTask m_task;
        sf::Sprite m_webSprite;
        sf::Sprite m_spiderSprite;
        float m_animElapsedSec;
        std::size_t m_frameIndex;
        std::size_t m_hitPoints;
        sf::FloatRect m_mapRect;
        sf::FloatRect m_webRect;
        sf::Vector2f m_sitPosition;
        bool m_hasDeathAnimFinished;
        float m_descendSpeed;
        const std::vector<std::vector<std::vector<sf::Texture>>> & m_texturesVecVec;
    };

} // namespace halloween

#endif // SPIDER_HPP_INCLUDED
