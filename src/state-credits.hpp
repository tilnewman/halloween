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

    class Credit : public sf::Drawable
    {
      public:
        Credit(
            const Context & context,
            const std::string & name,
            const std::string & desc,
            const std::string & license = "",
            const std::string & extra = "");

        void update(const float frameTimeSec);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;
        void vertPosition(const float pos);
        float bottom() const;

      private:
        sf::Text m_nameText;
        sf::Text m_descText;
        sf::Text m_licenseText;
        sf::Text m_extraText;
        static const float m_vertPad;
    };

    //

    class StateCredits : public StateBase
    {
      public:
        StateCredits();
        virtual ~StateCredits() final {}

        void onEnter(Context & context) final;
        void update(Context & context, const float frameTimeSec) final;
        void draw(const Context & con, sf::RenderTarget & tar, sf::RenderStates & sta) const final;
        bool handleEvent(Context & context, const sf::Event & event) final;

      private:
        std::vector<Credit> m_credits;
    };

} // namespace halloween

#endif // STATECREDITS_HPP_INCLUDED
