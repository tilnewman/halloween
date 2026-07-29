#ifndef STATECREDITS_HPP_INCLUDED
#define STATECREDITS_HPP_INCLUDED
//
// state-credits.hpp
//
#include "states.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>
#include <vector>

namespace halloween
{
    struct Context;

    //

    class Credit : public sf::Drawable
    {
      public:
        Credit(
            const Context & t_context,
            const std::string & t_name,
            const std::string & t_desc,
            const std::string & t_license = "",
            const std::string & t_extra = "");

        void update(const Context & t_context, const float t_frameTimeSec);
        void draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const override;
        void vertPosition(const float t_position);
        float bottom() const;

      private:
        sf::Text m_nameText;
        sf::Text m_descText;
        sf::Text m_licenseText;
        sf::Text m_extraText;
        inline static const float m_vertPad{ 10.0f };
    };

    //

    class StateCredits final : public StateBase
    {
      public:
        StateCredits();
        virtual ~StateCredits() final {}

        void onEnter(const Context & t_context) final;
        void onExit(const Context & t_context) final;
        void update(const Context & t_context, const float t_frameTimeSec) final;

        void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates & t_states) const final;

        bool handleEvent(const Context & t_context, const sf::Event & t_event) final;

      private:
        std::vector<Credit> m_credits;
    };

} // namespace halloween

#endif // STATECREDITS_HPP_INCLUDED
