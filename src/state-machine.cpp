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
#include "state-trader.hpp"

namespace halloween
{
    StateMachine::StateMachine()
        : m_stateUPtr{ makeState(State::Start) }
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

        m_stateUPtr = makeState(m_changePendingOpt.value());
        m_changePendingOpt = std::nullopt;

        m_stateUPtr->onEnter(t_context);
    }

    IStateUPtr_t StateMachine::makeState(const State t_state)
    {
        // clang-format off
        switch (t_state)
        {
            case State::Start:    { return std::make_unique<StartState>();         }
            case State::Title:    { return std::make_unique<TitleState>();         }
            case State::Play:     { return std::make_unique<PlayState>();          }
            case State::Pause:    { return std::make_unique<PauseState>();         }
            case State::Level:    { return std::make_unique<LevelCompleteState>(); }
            case State::Lose:     { return std::make_unique<LoseState>();          }
            case State::Win:      { return std::make_unique<WinState>();           }
            case State::Credits:  { return std::make_unique<StateCredits>();       }
            case State::Quit:     { return std::make_unique<QuitState>();          }
            case State::Trader:   { return std::make_unique<TraderState>();        }
            default:
            {
                M_LOG("Asked to make a state that is not implemented.  Til, you forgot "\
                      "to update the makeState() function after adding a new state again...");

                assert(false);
                return std::make_unique<QuitState>();            
            } 
        };
        // clang-format on
    }

} // namespace halloween
