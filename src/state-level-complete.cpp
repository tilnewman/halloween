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
        , m_elapsedScoreTimeSec(0.0f)
        , m_timeBetweenScoreUpdateSec(0.05f)
        , m_scoreDisplayed(0)
        , m_hasScoreFinishedUpdating(false)
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

        m_bonusText = context.media.makeText(70, "", sf::Color(255, 255, 153));

        std::string bonusStr;

        const bool collectedAllCoins = (context.stats.coin_total == context.stats.coin_collected);
        if (collectedAllCoins)
        {
            context.info_region.scoreAdjust(100);

            bonusStr += "All Coins Found Bonus!";
        }

        const bool killedAllEnemies = (context.stats.enemy_total == context.stats.enemy_killed);
        if (killedAllEnemies)
        {
            context.info_region.scoreAdjust(100);

            if (!bonusStr.empty())
            {
                bonusStr += '\n';
            }

            bonusStr += "All Enemies Killed Bonus!";
        }

        const bool stayedAlive = !context.stats.has_player_died;
        if (stayedAlive)
        {
            context.info_region.scoreAdjust(100);

            if (!bonusStr.empty())
            {
                bonusStr += '\n';
            }

            bonusStr += "You Didn't Die Bonus!";
        }

        if (collectedAllCoins && killedAllEnemies && stayedAlive)
        {
            context.info_region.scoreAdjust(1000);

            if (!bonusStr.empty())
            {
                bonusStr += '\n';
            }

            bonusStr += "PERFECT!!!";
        }

        if (bonusStr.empty())
        {
            bonusStr = "No bonuses, lame.";
        }

        m_bonusText.setString(bonusStr);
        util::centerInside(m_bonusText, context.layout.mapRegion());
        util::setOriginToPosition(m_bonusText);

        m_bonusText.setPosition(
            m_bonusText.getPosition().x,
            (util::bottom(context.layout.mapRegion()) - m_bonusText.getGlobalBounds().height));
    }

    void LevelCompleteState::updateScoreText(const Context & context)
    {
        std::string scoreStr{ "Score: " };
        scoreStr += std::to_string(m_scoreDisplayed);
        m_scoreText.setString(scoreStr);
        util::centerInside(m_scoreText, context.layout.wholeRegion());
    }

    void LevelCompleteState::onExit(Context & context)
    {
        ++context.level_number;
        context.managers.clearAll();
    }

    bool LevelCompleteState::handleEvent(Context &, const sf::Event &)
    {
        // always returning false prevents the player from quiting the state early
        return false;
    }

    void LevelCompleteState::update(Context & context, const float frameTimeSec)
    {
        m_hasScoreFinishedUpdating = (m_scoreDisplayed == context.info_region.score());

        m_elapsedTimeSec += frameTimeSec;
        if (m_hasScoreFinishedUpdating && (m_elapsedTimeSec > 6.0f))
        {
            context.state.setChangePending(State::Play);
        }

        m_elapsedScoreTimeSec += frameTimeSec;
        if ((m_scoreDisplayed != context.info_region.score()) &&
            (m_elapsedScoreTimeSec > m_timeBetweenScoreUpdateSec))
        {
            m_elapsedScoreTimeSec -= m_timeBetweenScoreUpdateSec;

            ++m_scoreDisplayed;
            updateScoreText(context);
            context.audio.play("bell");

            m_hasScoreFinishedUpdating = (m_scoreDisplayed == context.info_region.score());
            if (m_hasScoreFinishedUpdating)
            {
                m_elapsedTimeSec = 0.0f;
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
