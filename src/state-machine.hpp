#ifndef STATESMACHINE_HPP_INCLUDED
#define STATESMACHINE_HPP_INCLUDED
//
// states.hpp
//
#include "states.hpp"

//

namespace sf
{
    class Event;
    class RenderTarget;
    class RenderStates;
} // namespace sf

//

namespace halloween
{
    struct Context;

    //

    struct IStatesPending
    {
        virtual ~IStatesPending() = default;

        virtual bool isChangePending() const = 0;
        virtual StateOpt_t getChangePending() const = 0;
        virtual void setChangePending(const State state) = 0;
    };

    //

    class StateMachine : public IStatesPending
    {
      public:
        StateMachine();
        ~StateMachine() override = default;

        // prevent all copy and assignment
        StateMachine(const StateMachine &) = delete;
        StateMachine(StateMachine &&) = delete;
        //
        StateMachine & operator=(const StateMachine &) = delete;
        StateMachine & operator=(StateMachine &&) = delete;

        void reset();

        inline State stateEnum() const { return m_stateUPtr->state(); }

        inline IState & state() { return *m_stateUPtr; }
        inline const IState & state() const { return *m_stateUPtr; }

        inline bool isChangePending() const override { return m_changePendingOpt.has_value(); }
        inline StateOpt_t getChangePending() const override { return m_changePendingOpt; }
        void setChangePending(const State state) override;
        void changeIfPending(Context & context);

      private:
        static IStateUPtr_t makeState(Context & context, const State state);

      private:
        IStateUPtr_t m_stateUPtr;
        StateOpt_t m_changePendingOpt;
    };

} // namespace halloween

#endif // STATESMACHINE_HPP_INCLUDED
