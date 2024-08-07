// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// state-level-complete.cpp
//
#include "state-level-complete.hpp"

#include "context.hpp"
#include "info-region.hpp"
#include "level-stats.hpp"
#include "object-manager.hpp"
#include "resources.hpp"
#include "screen-regions.hpp"
#include "sfml-util.hpp"
#include "sound-player.hpp"
#include "state-machine.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

namespace halloween
{

    LevelCompleteState::LevelCompleteState(const Context & context)
        : StateBase(context, State::Level, State::Play)
        , m_levelCompleteText()
        , m_scoreText()
        , m_bonusText()
        , m_bonuses()
        , m_isPreWaiting(true)
        , m_isShowingBonuses(false)
        , m_isPostWaiting(false)
        , m_timeBetweenScoreUpdateSec(0.05f)
        , m_scoreDisplayed(0)
    {}

    void LevelCompleteState::onEnter(Context & context)
    {
        context.audio.play("level-complete");

        m_levelCompleteText = context.media.makeText(99, "Level Complete!\n\n", m_textColorDefault);

        util::fitAndCenterInside(
            m_levelCompleteText, util::scaleRectInPlaceCopy(context.layout.wholeRegion(), 0.25f));

        m_scoreDisplayed = context.info_region.score();

        m_scoreText = context.media.makeText(50, "", sf::Color(160, 160, 160));

        updateScoreText(context);

        m_bonusTextRegion.left = 0.0f;
        m_bonusTextRegion.top = util::bottom(m_scoreText);
        m_bonusTextRegion.width = context.layout.wholeRegion().width;

        m_bonusTextRegion.height =
            ((context.layout.wholeRegion().height - m_bonusTextRegion.top) -
             (context.layout.wholeSize().y * 0.25f));

        const unsigned bonusTextCharSize{ 70 };
        const sf::Color bonusTextColor{ 255, 255, 153 };

        const bool willCoinBonus{ context.stats.coin_total == context.stats.coin_collected };
        const bool willEnemyBonus{ context.stats.enemy_total == context.stats.enemy_killed };
        const bool willSurviveBonus{ !context.stats.has_player_died };
        const bool willPerfectBonus{ willCoinBonus && willEnemyBonus && willSurviveBonus };

        if (willPerfectBonus)
        {
            m_bonuses.emplace_back(
                1000, context.media.makeText(bonusTextCharSize, "Perfect!", bonusTextColor));
        }

        if (willCoinBonus)
        {
            m_bonuses.emplace_back(
                99,
                context.media.makeText(
                    bonusTextCharSize, "All Coins Found Bonus!", bonusTextColor));
        }

        if (willEnemyBonus)
        {
            m_bonuses.emplace_back(
                50,
                context.media.makeText(
                    bonusTextCharSize, "All Enemies Killed Bonus!", bonusTextColor));
        }

        if (willSurviveBonus)
        {
            m_bonuses.emplace_back(
                75,
                context.media.makeText(bonusTextCharSize, "You Didn't Die Bonus!", bonusTextColor));
        }

        if (m_bonuses.size() == 3)
        {
            m_bonuses.emplace_back(
                0, context.media.makeText(bonusTextCharSize, "No bonuses, lame.", bonusTextColor));
        }
    }

    void LevelCompleteState::updateScoreText(const Context & context)
    {
        std::string scoreStr{ "Score: " };
        scoreStr += std::to_string(m_scoreDisplayed);
        m_scoreText.setString(scoreStr);
        util::centerInside(m_scoreText, context.layout.wholeRegion());
    }

    void LevelCompleteState::onExit(Context & context) { ++context.level_number; }

    bool LevelCompleteState::handleEvent(Context &, const sf::Event &)
    {
        // always returning false prevents the player from quiting the state early
        return false;
    }

    bool LevelCompleteState::popAndDisplayNextBonus(Context & context)
    {
        if (m_bonuses.empty())
        {
            return false;
        }

        context.info_region.scoreAdjust(m_bonuses.back().score);
        m_bonusText = m_bonuses.back().text;
        util::centerInside(m_bonusText, m_bonusTextRegion);

        m_bonuses.resize(m_bonuses.size() - 1);
        return true;
    }

    void LevelCompleteState::update(Context & context, const float frameTimeSec)
    {
        auto hasScoreFinishedUpdating = [&]() {
            return (m_scoreDisplayed == context.info_region.score());
        };

        if (m_isPreWaiting)
        {
            m_elapsedTimeSec += frameTimeSec;
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

                if (popAndDisplayNextBonus(context))
                {
                    context.audio.play("bonus");
                }
                else
                {
                    m_isShowingBonuses = false;
                    m_isPostWaiting = true;
                }
            }
            else
            {
                m_elapsedTimeSec += frameTimeSec;
                if (m_elapsedTimeSec > m_timeBetweenScoreUpdateSec)
                {
                    m_elapsedTimeSec -= m_timeBetweenScoreUpdateSec;

                    int scoreAdjustment = ((context.info_region.score() - m_scoreDisplayed) / 10);
                    if (0 == scoreAdjustment)
                    {
                        scoreAdjustment = 1;
                    }

                    m_scoreDisplayed += scoreAdjustment;
                    updateScoreText(context);
                    context.audio.play("bell");
                }
            }
        }

        if (m_isPostWaiting)
        {
            m_elapsedTimeSec += frameTimeSec;
            if (m_elapsedTimeSec > 6.0f)
            {
                context.state.setChangePending(State::Play);
            }
        }
    }

    void LevelCompleteState::draw(
        const Context &, sf::RenderTarget & target, sf::RenderStates & states) const
    {
        target.draw(m_levelCompleteText, states);
        target.draw(m_scoreText, states);
        target.draw(m_bonusText, states);
    }

} // namespace halloween
