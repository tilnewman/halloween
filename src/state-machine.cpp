// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// state-machine.cpp
//
#include "state-machine.hpp"

#include "check-macros.hpp"
#include "context.hpp"
#include "state-credits.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

namespace halloween
{
    StateMachine::StateMachine()
        : m_stateUPtr()
        , m_changePendingOpt(State::Start)
    {
        reset();
    }

    void StateMachine::reset()
    {
        m_stateUPtr = std::make_unique<StartState>();
        m_changePendingOpt = m_stateUPtr->state();
    }

    void StateMachine::setChangePending(const State state) { m_changePendingOpt = state; }

    void StateMachine::changeIfPending(Context & context)
    {
        if (!m_changePendingOpt)
        {
            return;
        }

        m_stateUPtr->onExit(context);

        m_stateUPtr = makeState(context, m_changePendingOpt.value());
        m_changePendingOpt = std::nullopt;

        m_stateUPtr->onEnter(context);
    }

    IStateUPtr_t StateMachine::makeState(Context & context, const State state)
    {
        // clang-format off
        switch (state)
        {
            case State::Start:    { return std::make_unique<StartState>();                }
            case State::Title:    { return std::make_unique<TitleState>(context);         }
            case State::Play:     { return std::make_unique<PlayState>(context);          }
            case State::Pause:    { return std::make_unique<PauseState>(context);         }
            case State::Level:    { return std::make_unique<LevelCompleteState>(context); }
            case State::GameOver: { return std::make_unique<GameOverState>(context);      }
            case State::Credits:  { return std::make_unique<StateCredits>();              }
            case State::Quit:     { return std::make_unique<QuitState>();                 }
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
