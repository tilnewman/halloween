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
    {}

    void LevelCompleteState::onEnter(Context & context)
    {
        context.audio.play("level-complete");

        m_levelCompleteText.setString("Level Complete!\n\n");
        m_levelCompleteText.setCharacterSize(99);
        m_levelCompleteText.setFont(context.media.font);
        m_levelCompleteText.setFillColor(m_textColorDefault);

        util::fitAndCenterInside(
            m_levelCompleteText, util::scaleRectInPlaceCopy(context.layout.wholeRegion(), 0.25f));

        std::string scoreStr{ "Score: " };
        scoreStr += std::to_string(context.info_region.score());

        // if (context.stats.coin_total == context.stats.coin_collected)
        //{
        //    scoreStr += "\ncoins_total=";
        //    scoreStr += std::to_string(context.stats.coin_total);
        //    scoreStr += ", collected=";
        //    scoreStr += std::to_string(context.stats.coin_collected);
        //}

        // if (context.stats.enemy_total == context.stats.enemy_killed)
        //{
        //    scoreStr += "\nenemy_total=";
        //    scoreStr += std::to_string(context.stats.enemy_total);
        //    scoreStr += ", killed=";
        //    scoreStr += std::to_string(context.stats.enemy_killed);
        //}

        m_scoreText = m_levelCompleteText;
        m_scoreText.setCharacterSize(50);
        m_scoreText.setString(scoreStr);
        m_scoreText.setFillColor(sf::Color(127, 127, 127));
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
        m_elapsedTimeSec += frameTimeSec;
        if (m_elapsedTimeSec > 12.0f)
        {
            context.state.setChangePending(State::Play);
        }
    }

    void LevelCompleteState::draw(
        const Context &, sf::RenderTarget & target, sf::RenderStates & states) const
    {
        target.draw(m_levelCompleteText, states);
        target.draw(m_scoreText, states);
    }

} // namespace halloween
