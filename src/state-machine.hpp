#ifndef STATESMACHINE_HPP_INCLUDED
#define STATESMACHINE_HPP_INCLUDED
//
// state-machine.hpp
//
#include "states.hpp"

namespace halloween
{
    struct Context;

    //

    class StateMachine
    {
      public:
        StateMachine();

        // prevent all copy and assignment
        StateMachine(const StateMachine &) = delete;
        StateMachine(StateMachine &&) = delete;
        //
        StateMachine & operator=(const StateMachine &) = delete;
        StateMachine & operator=(StateMachine &&) = delete;

        inline State stateEnum() const { return m_stateUPtr->state(); }

        inline IState & state() { return *m_stateUPtr; }
        inline const IState & state() const { return *m_stateUPtr; }

        inline bool isChangePending() const  { return m_changePendingOpt.has_value(); }
        inline StateOpt_t getChangePending() const  { return m_changePendingOpt; }
        void setChangePending(const State t_state);
        void changeIfPending(const Context & t_context);

      private:
        static IStateUPtr_t makeState(const State t_state);

      private:
        IStateUPtr_t m_stateUPtr;
        StateOpt_t m_changePendingOpt;
    };

} // namespace halloween

#endif // STATESMACHINE_HPP_INCLUDED
