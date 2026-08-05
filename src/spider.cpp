// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// spider.cpp
//
#include "spider.hpp"

#include "avatar.hpp"
#include "check-macros.hpp"
#include "context.hpp"
#include "random.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-defaults.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace halloween
{
    Spider::Spider(
        const Context & t_context,
        const sf::FloatRect & t_rect,
        const sf::Texture & t_webTexture,
        const std::vector<std::vector<std::vector<sf::Texture>>> & t_texturesVecVec)
        : m_anim{ SpiderAnim::Idle }
        , m_type{ t_context.random.from({ SpiderType::Mom, SpiderType::Dad, SpiderType::Child }) }
        , m_task{ SpiderTask::Descend }
        , m_webSprite{ t_webTexture }
        , m_spiderSprite{ util::SfmlDefaults::instance().texture() }
        , m_animElapsedSec{ 0.0f }
        , m_frameIndex{ 0 }
        , m_hitPoints{ 1 }
        , m_mapRect{ t_rect }
        , m_webRect{ t_rect.position, { t_rect.size.x, t_rect.size.x } }
        , m_sitPosition{}
        , m_hasDeathAnimFinished{ false }
        , m_descendSpeed{ t_context.random.fromTo(20.0f, 40.0f) }
        , m_texturesVecVec{ t_texturesVecVec }
    {
        //
        util::setOriginToCenter(m_webSprite);
        util::fitAndCenterInside(m_webSprite, m_webRect);
        m_webSprite.setPosition(util::center(m_webRect));
        m_webSprite.setRotation(sf::degrees(t_context.random.fromTo(0.0f, 360.0f)));
        m_webSprite.setColor(sf::Color(255, 255, 255, 127));
        m_sitPosition = m_webSprite.getPosition();

        //
        m_spiderSprite.setTexture(getTextures(m_type, m_anim).at(0), true);
        util::setOriginToCenter(m_spiderSprite);
        const float scale{ (SpiderType::Child == m_type) ? 0.4f : 0.25f };
        m_spiderSprite.scale({ scale, scale });
        m_spiderSprite.setPosition(m_sitPosition);

        if (t_context.random.boolean())
        {
            m_spiderSprite.scale({ -1.0f, 1.0f });
        }
    }

    const sf::FloatRect Spider::collisionRect() const
    {
        return util::scaleRectInPlaceCopy(m_spiderSprite.getGlobalBounds(), { 0.4f, 0.65f });
    }

    void Spider::setupTask(const SpiderTask t_task, const SpiderAnim t_anim)
    {
        m_task = t_task;
        m_anim = t_anim;
        m_animElapsedSec = 0.0f;
        m_frameIndex = 0;
    }

    void Spider::update(const Context & t_context, const float t_frameTimeSec)
    {
        // stay waiting until the player comes close enough
        if (SpiderTask::Wait == m_task)
        {
            const sf::FloatRect avatarRect{ t_context.avatar.collisionRect() };

            const float horizDistanceToPlayer{ std::abs(
                util::center(m_webRect).x - util::right(avatarRect)) };

            const float heightFromGround{ util::bottom(m_mapRect) - util::center(m_webRect).y };

            if (horizDistanceToPlayer < heightFromGround)
            {
                // TODO play 'spider spots the player' sfx
                setupTask(SpiderTask::Descend, SpiderAnim::Idle);
            }

            return;
        }

        // animate
        m_animElapsedSec += t_frameTimeSec;
        const float timePerFrame{ timePerFrameSec(m_anim) };
        if (m_animElapsedSec > timePerFrame)
        {
            m_animElapsedSec -= timePerFrame;

            const auto & textures{ getTextures(m_type, m_anim) };
            if (++m_frameIndex >= textures.size())
            {
                m_frameIndex = 0;

                if (SpiderAnim::Death == m_anim)
                {
                    m_hasDeathAnimFinished = true;
                }
            }

            changeTextureWithoutMovingSprite(textures.at(m_frameIndex));
        }

        // update tasks
        if (SpiderTask::Descend == m_task)
        {
            m_spiderSprite.move({ 0.0f, (m_descendSpeed * t_frameTimeSec) });

            const float distFromGround{ std::abs(
                util::bottom(m_mapRect) - util::bottom(m_spiderSprite.getGlobalBounds())) };

            if (distFromGround < 5.0f)
            {
                setupTask(SpiderTask::Ascend, SpiderAnim::Move);
            }
        }
        else if (SpiderTask::Ascend == m_task)
        {
            const float ascendSpeed{ m_descendSpeed * -2.0f };
            m_spiderSprite.move({ 0.0f, (ascendSpeed * t_frameTimeSec) });

            const float distFromWeb{ std::abs(
                util::center(m_spiderSprite.getGlobalBounds()).y - m_sitPosition.y) };

            if (distFromWeb < 5.0f)
            {
                setupTask(SpiderTask::Wait, SpiderAnim::Idle);
                changeTextureWithoutMovingSprite(getTextures(m_type, m_anim).at(0));
            }
        }
    }

    // some of the spider animations are different png sizes, so we have to re-center when changing
    void Spider::changeTextureWithoutMovingSprite(const sf::Texture & t_texture)
    {
        const sf::Vector2f spiderPos{ m_spiderSprite.getPosition() };
        m_spiderSprite.setTexture(t_texture, true);
        util::setOriginToCenter(m_spiderSprite);
        m_spiderSprite.setPosition(spiderPos);
    }

    void Spider::moveWithMap(const sf::Vector2f & t_move)
    {
        m_webSprite.move(t_move);
        m_spiderSprite.move(t_move);
        m_webRect.position += t_move;
        m_mapRect.position += t_move;
        m_sitPosition += t_move;
    }

    bool Spider::doesAvatarCollideWithAnyAndDie(const Context &, const sf::FloatRect & t_avatarRect)
    {
        if (isAlive() and t_avatarRect.findIntersection(collisionRect()))
        {
            setupTask(SpiderTask::Ascend, SpiderAnim::Move);
            return true;
        }
        else
        {
            return false;
        }
    }

    void Spider::hit(const Context & t_context)
    {
        t_context.audio.play("metal-hit");

        if (m_hitPoints > 0)
        {
            --m_hitPoints;

            if (not isAlive())
            {
                t_context.audio.play("spider-death");
                setupTask(SpiderTask::Death, SpiderAnim::Death);
            }
            else
            {
                // TODO t_context.audio.play("spider-hit");
                // setupTask(SpiderTask::Hit, ZombieAnim::Hit);
            }
        }
    }

    void Spider::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        if (t_context.layout.wholeRegion().findIntersection(m_webSprite.getGlobalBounds()))
        {
            t_target.draw(m_webSprite, t_states);

            if (not m_hasDeathAnimFinished)
            {
                if (isAlive())
                {
                    const sf::FloatRect strandRect(
                        m_sitPosition,
                        { 0.0f, (m_spiderSprite.getPosition().y - m_sitPosition.y) });

                    util::drawRectangleShape(
                        t_target, strandRect, false, sf::Color(255, 255, 255, 92));
                }

                t_target.draw(m_spiderSprite, t_states);

                // util::drawRectangleShape(t_target, collisionRect(), false, sf::Color::Red);
                // util::drawRectangleShape(t_target, attackRect(), false, sf::Color::Yellow);
            }
        }
    }

    const std::vector<sf::Texture> &
        Spider::getTextures(const SpiderType t_type, const SpiderAnim t_action) const
    {
        const std::size_t typeIndex{ static_cast<std::size_t>(t_type) };

        M_CHECK(
            (typeIndex < m_texturesVecVec.size()),
            "textures(" << toString(t_type) << ", " << toString(t_action)
                        << ") when t_type=" << typeIndex << " is out of range!");

        const std::vector<std::vector<sf::Texture>> & textureActions{ m_texturesVecVec.at(
            typeIndex) };

        const std::size_t actionIndex{ static_cast<std::size_t>(t_action) };

        M_CHECK(
            (actionIndex < textureActions.size()),
            "textures(" << toString(t_type) << ", " << toString(t_action)
                        << ") when t_action=" << actionIndex << " is out of range!");

        return textureActions.at(actionIndex);
    }

} // namespace halloween
