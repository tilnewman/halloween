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

    // TODO is this interface really needed anymore?
    struct IStatesPending
    {
        virtual ~IStatesPending() = default;

        virtual bool isChangePending() const = 0;
        virtual StateOpt_t getChangePending() const = 0;
        virtual void setChangePending(const State t_state) = 0;
    };

    //

    class StateMachine final : public IStatesPending
    {
      public:
        StateMachine();
        ~StateMachine() final = default;

        // prevent all copy and assignment
        StateMachine(const StateMachine &) = delete;
        StateMachine(StateMachine &&) = delete;
        //
        StateMachine & operator=(const StateMachine &) = delete;
        StateMachine & operator=(StateMachine &&) = delete;

        inline State stateEnum() const { return m_stateUPtr->state(); }

        inline IState & state() { return *m_stateUPtr; }
        inline const IState & state() const { return *m_stateUPtr; }

        inline bool isChangePending() const final { return m_changePendingOpt.has_value(); }
        inline StateOpt_t getChangePending() const final { return m_changePendingOpt; }
        void setChangePending(const State t_state) final;
        void changeIfPending(const Context & t_context);

      private:
        static IStateUPtr_t makeState(const Context & t_context, const State t_state);

      private:
        IStateUPtr_t m_stateUPtr;
        StateOpt_t m_changePendingOpt;
    };

} // namespace halloween

#endif // STATESMACHINE_HPP_INCLUDED
