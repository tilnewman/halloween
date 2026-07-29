// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// state-machine.cpp
//
#include "state-machine.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "state-credits.hpp"
#include "state-level-complete.hpp"
#include "state-play.hpp"

namespace halloween
{
    StateMachine::StateMachine()
        : m_stateUPtr{ std::make_unique<StartState>() }
        , m_changePendingOpt{ State::Start }
    {}

    void StateMachine::setChangePending(const State t_state) { m_changePendingOpt = t_state; }

    void StateMachine::changeIfPending(const Context & t_context)
    {
        if (!m_changePendingOpt)
        {
            return;
        }

        m_stateUPtr->onExit(t_context);

        m_stateUPtr = makeState(t_context, m_changePendingOpt.value());
        m_changePendingOpt = std::nullopt;

        m_stateUPtr->onEnter(t_context);
    }

    IStateUPtr_t StateMachine::makeState(const Context & t_context, const State t_state)
    {
        // clang-format off
        switch (t_state)
        {
            case State::Start:    { return std::make_unique<StartState>();                  }
            case State::Title:    { return std::make_unique<TitleState>(t_context);         }
            case State::Play:     { return std::make_unique<PlayState>(t_context);          }
            case State::Pause:    { return std::make_unique<PauseState>(t_context);         }
            case State::Level:    { return std::make_unique<LevelCompleteState>(t_context); }
            case State::Lose:     { return std::make_unique<LoseState>(t_context);          }
            case State::Win:      { return std::make_unique<WinState>(t_context);           }
            case State::Credits:  { return std::make_unique<StateCredits>();                }
            case State::Quit:     { return std::make_unique<QuitState>();                   }
            default:
            {
                M_LOG("Asked to make a state that is not implemented.  Til, you forgot"\
                      "to update the makeState() function after adding a new state again...");

                assert(false);
                return std::make_unique<QuitState>();            
            } 
        };
        // clang-format on
    }

} // namespace halloween
