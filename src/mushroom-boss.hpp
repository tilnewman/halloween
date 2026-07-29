#ifndef MUSHROOMBOSS_HPP_INCLUDED
#define MUSHROOMBOSS_HPP_INCLUDED
//
// mushroom-boss.hpp
//
#include "avatar-anim.hpp"
#include "object-manager.hpp"

#include <cassert>
#include <string_view>
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace halloween
{
    struct Context;
    struct Settings;

    //

    struct BossCollRects
    {
        sf::FloatRect top{};
        sf::FloatRect middle{};
        sf::FloatRect bottom{};
    };

    //

    enum class BossState : std::size_t
    {
        Idle = 0,
        Advance,
        Jump,
        Attack,
        Hit,
        Shake,
        Death
    };

    constexpr std::string_view toString(const BossState t_state) noexcept
    {
        // clang-format off
        switch (t_state)
        {
            case BossState::Idle:   { return "Idle";            }
            case BossState::Advance:{ return "Advance";         }
            case BossState::Jump:   { return "Jump";            }
            case BossState::Attack: { return "Attack";          }
            case BossState::Hit:    { return "Hit";             }
            case BossState::Shake:  { return "Shake";           }
            case BossState::Death:  { return "Death";           }
            default:              { return "Unknown_BossState"; }
        }
        // clang-format on
    }

    //

    class MushroomBoss : public IObjectManager
    {
      public:
        MushroomBoss();
        virtual ~MushroomBoss() override = default;

        bool willDrawBeforeMap() const final { return false; }
        void setup(const Settings & t_settings) final;
        void add(const Context & t_context, const sf::FloatRect & t_rect) final;
        void clear() final;
        void update(const Context & t_context, const float t_frameTimeSec) final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

        void moveWithMap(const sf::Vector2f & t_move) final;
        void collideWithAvatar(const Context & t_context, const sf::FloatRect & t_avatarRect) final;
        bool doesAvatarCollideWithAnyAndDie(const sf::FloatRect &) const final { return false; }
        void appendCollisions(std::vector<sf::FloatRect> &) const final {}

        bool attack(const Context & context, const sf::FloatRect & t_attackRect);
        bool doesCollide(const sf::FloatRect & t_rect) const;
        void reactToThrow(const Context & t_context);

      private:
        void setState(const BossState t_newState);
        AvatarAnim & currentAnim();
        const BossCollRects collisionRects() const;
        void keepInRegion();
        bool isPlayerWithinAttackRange(const Context & t_context, const bool t_isHitQuery) const;

      private:
        BossState m_state;
        AvatarAnim m_idleAnim;
        AvatarAnim m_jumpAnim;
        AvatarAnim m_attackAnim;
        AvatarAnim m_hitAnim;
        AvatarAnim m_shakeAnim;
        AvatarAnim m_deathAnim;
        sf::Sprite m_sprite;
        sf::FloatRect m_region;
        bool m_isThereABossOnThisLevel;
        bool m_hasFightBegun;
        std::size_t m_hitPoints;
        std::size_t m_hitPointsMax;
    };

} // namespace halloween

#endif // MUSHROOMBOSS_HPP_INCLUDED
