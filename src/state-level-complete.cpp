// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// state-level-complete.cpp
//
#include "state-level-complete.hpp"

#include "context.hpp"
#include "fonts.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "level.hpp"
#include "object-manager.hpp"
#include "screen-regions.hpp"
#include "settings.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"
#include "state-machine.hpp"
#include "state-play.hpp"
#include "texture-loader.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

namespace halloween
{

    LevelCompleteState::LevelCompleteState()
        : StateBase{ State::Level, State::Play }
        , m_strawMatTexture{}
        , m_strawMatSprite{ m_strawMatTexture }
        , m_panelTexture{}
        , m_panelSprite{ m_panelTexture }
        , m_levelCompleteText{ util::SfmlDefaults::instance().font() }
        , m_scoreText{ util::SfmlDefaults::instance().font() }
        , m_bonusText{ util::SfmlDefaults::instance().font() }
        , m_bonuses{}
        , m_bonusTextRegion{}
        , m_isPreWaiting{ true }
        , m_isShowingBonuses{ false }
        , m_isPostWaiting{ false }
        , m_timeBetweenScoreUpdateSec{ 0.05f }
        , m_scoreDisplayed{ 0 }
    {}

    void LevelCompleteState::onEnter(const Context & t_context)
    {
        StateBase::onEnter(t_context);

        t_context.audio.play("level-complete");

        //
        const sf::FloatRect wholeRect{ t_context.layout.wholeRegion() };

        util::TextureLoader::load(
            m_strawMatTexture, (t_context.settings.media_path / "image" / "straw-mat.png"), true);

        m_strawMatTexture.setRepeated(true);

        m_strawMatSprite.setTexture(m_strawMatTexture, true);
        m_strawMatSprite.setTextureRect({ { 0, 0 }, sf::Vector2i{ wholeRect.size } });
        m_strawMatSprite.setColor(sf::Color(255, 255, 255, 127));

        //
        util::TextureLoader::load(
            m_panelTexture, (t_context.settings.media_path / "image" / "wood-panel.png"), true);

        m_panelTexture.setRepeated(true);

        m_panelSprite.setTexture(m_panelTexture, true);

        sf::IntRect panelRect;
        panelRect.position.x = 0;
        panelRect.position.y = 0;
        panelRect.size.x = static_cast<int>(wholeRect.size.x);
        panelRect.size.y = static_cast<int>(m_panelTexture.getSize().y);
        m_panelSprite.setTextureRect(panelRect);

        m_panelSprite.setPosition({ 0.0f, static_cast<float>(m_panelTexture.getSize().y * 2) });

        //
        const sf::FloatRect panelContextRect{ { 0.0f, (m_panelSprite.getPosition().y + 30.0f) },
                                              { wholeRect.size.x, 210.0f } };

        //
        m_levelCompleteText = t_context.fonts.makeText(99, "Level Complete!", m_textColorDefault);

        m_levelCompleteText.setPosition(
            { (util::center(wholeRect).x - util::center(m_levelCompleteText.getGlobalBounds()).x),
              (panelContextRect.position.y + 5.0f) });

        m_scoreDisplayed = t_context.info_region.score();

        m_scoreText = t_context.fonts.makeText(50, "", sf::Color(160, 160, 160));

        updateScoreText(t_context);

        m_bonusTextRegion.position.x = 0.0f;

        m_bonusTextRegion.position.y =
            (util::bottom(panelContextRect) - m_levelCompleteText.getGlobalBounds().size.y);

        m_bonusTextRegion.size.x = t_context.layout.wholeRegion().size.x;
        m_bonusTextRegion.size.y = m_levelCompleteText.getGlobalBounds().size.y;

        const unsigned bonusTextCharSize{ 70 };
        const sf::Color bonusTextColor{ 255, 255, 153 };

        const bool willCoinBonus{ (t_context.stats.coin_total > 0) &&
                                  (t_context.stats.coin_total == t_context.stats.coin_collected) };

        const bool willEnemyBonus{ (t_context.stats.enemy_total > 0) &&
                                   (t_context.stats.enemy_total == t_context.stats.enemy_killed) };

        const bool willSurviveBonus{ !t_context.stats.has_player_died };
        const bool willPerfectBonus{ willCoinBonus && willEnemyBonus && willSurviveBonus };

        if (willPerfectBonus)
        {
            m_bonuses.emplace_back(
                200, t_context.fonts.makeText(bonusTextCharSize, "Perfect!", bonusTextColor));
        }

        if (willCoinBonus)
        {
            m_bonuses.emplace_back(
                100,
                t_context.fonts.makeText(
                    bonusTextCharSize, "All Coins Found Bonus!", bonusTextColor));
        }

        if (willEnemyBonus)
        {
            m_bonuses.emplace_back(
                50,
                t_context.fonts.makeText(
                    bonusTextCharSize, "All Enemies Killed Bonus!", bonusTextColor));
        }

        if (willSurviveBonus)
        {
            m_bonuses.emplace_back(
                75,
                t_context.fonts.makeText(
                    bonusTextCharSize, "You Didn't Die Bonus!", bonusTextColor));
        }

        if (m_bonuses.size() == 0)
        {
            m_bonuses.emplace_back(
                0,
                t_context.fonts.makeText(bonusTextCharSize, "No bonuses, lame.", bonusTextColor));
        }
    }

    void LevelCompleteState::updateScoreText(const Context & t_context)
    {
        std::string scoreStr{ "Score: " };
        scoreStr += std::to_string(m_scoreDisplayed);
        m_scoreText.setString(scoreStr);
        util::centerInside(m_scoreText, t_context.layout.wholeRegion());
        m_scoreText.move({ 0.0f, -(t_context.layout.wholeRegion().size.y * 0.025f) });
    }

    void LevelCompleteState::onExit(const Context & t_context)
    {
        if (t_context.level.number() < 3)
        {
            PlayState::m_willLoadNewLevel = true;
        }
        else
        {
            t_context.state.setChangePending(State::Win);
        }
    }

    bool LevelCompleteState::handleEvent(const Context & t_context, const sf::Event & t_event)
    {
        if (StateBase::handleEvent(t_context, t_event))
        {
            return true;
        }

        if (t_event.is<sf::Event::KeyPressed>())
        {
            if (!m_isPreWaiting && !m_isPostWaiting)
            {
                m_scoreDisplayed = t_context.info_region.score();
                updateScoreText(t_context);
                m_elapsedTimeSec += 9999.0f;
                t_context.audio.play("bell");
            }
        }

        return false;
    }

    bool LevelCompleteState::popAndDisplayNextBonus(const Context & t_context)
    {
        if (m_bonuses.empty())
        {
            return false;
        }

        t_context.info_region.scoreAdjust(m_bonuses.back().score);
        m_bonusText = m_bonuses.back().text;
        util::centerInside(m_bonusText, m_bonusTextRegion);

        m_bonuses.resize(m_bonuses.size() - 1);
        return true;
    }

    void LevelCompleteState::update(const Context & t_context, const float t_frameTimeSec)
    {
        auto hasScoreFinishedUpdating = [&]() {
            return (m_scoreDisplayed == t_context.info_region.score());
        };

        if (m_isPreWaiting)
        {
            m_elapsedTimeSec += t_frameTimeSec;
            if (m_elapsedTimeSec > 4.0f)
            {
                m_elapsedTimeSec = 0.0f;
                m_isPreWaiting = false;
                m_isShowingBonuses = true;
            }
        }

        if (m_isShowingBonuses)
        {
            if (hasScoreFinishedUpdating())
            {
                m_elapsedTimeSec = 0.0f;

                if (popAndDisplayNextBonus(t_context))
                {
                    t_context.audio.play("bonus");
                }
                else
                {
                    m_isShowingBonuses = false;
                    m_isPostWaiting = true;
                }
            }
            else
            {
                m_elapsedTimeSec += t_frameTimeSec;
                if (m_elapsedTimeSec > m_timeBetweenScoreUpdateSec)
                {
                    m_elapsedTimeSec -= m_timeBetweenScoreUpdateSec;

                    int scoreAdjustment = ((t_context.info_region.score() - m_scoreDisplayed) / 10);
                    if (0 == scoreAdjustment)
                    {
                        scoreAdjustment = 1;
                    }

                    m_scoreDisplayed += scoreAdjustment;
                    updateScoreText(t_context);
                    t_context.audio.play("bell");
                }
            }
        }

        if (m_isPostWaiting)
        {
            m_elapsedTimeSec += t_frameTimeSec;
            if (m_elapsedTimeSec > 6.0f)
            {
                t_context.state.setChangePending(State::Play);
            }
        }
    }

    void LevelCompleteState::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates & t_states) const
    {
        t_target.draw(m_strawMatSprite, t_states);
        t_target.draw(m_panelSprite, t_states);
        t_target.draw(m_levelCompleteText, t_states);
        t_target.draw(m_scoreText, t_states);
        t_target.draw(m_bonusText, t_states);
    }

} // namespace halloween
