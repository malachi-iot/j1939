#pragma once

#include <Arduino.h>

#include <estd/istream.h>
#include <estd/ostream.h>

// DEBT: Not available on AVR
#include <vector>
#include <estd/expected.h>


namespace menu {

class Action
{
protected:
    typedef estd::arduino_ostream ostream;

public:
    // Shows menu item entry/description for action
    virtual void render(ostream&) const = 0;

    virtual void action(ostream&) = 0;

    enum match_result
    {
        None,
        Partial,
        Match
    };

    // EXPERIMENTAL, see if we can get more complex than just number selection
    virtual match_result match(const char* input) { return None; }
};


class Menu;

class Navigator
{
    friend class Menu;

    Menu* const top_;
    Menu* current_;

    typedef estd::arduino_ostream istream;
    typedef estd::arduino_ostream ostream;

public:
    Navigator(Menu* top) :
        top_{top},
        current_{top}
    {}

    Menu* current() const { return current_; }
    void current(Menu* v) { current_ = v; }
    bool up();
    void poll(istream&, ostream&);
};


class Menu
{
public:
    typedef estd::arduino_ostream ostream;
    using errc = estd::errc;

    std::vector<Action*> items;
    
    Menu* const parent_ = nullptr;
    const char* const name_ = nullptr;

    Menu() = default;
    
    explicit Menu(const char* name, Menu* parent = nullptr) :
        parent_{parent},
        name_{name}
    {}

    estd::expected<void, errc> activate(int index, ostream& out)
    {
        // CTAD not a full option since we're c++11 mostly
        if(index < 0) return estd::unexpected<errc>(errc::invalid_argument);
        // FIX: We expect this brace init list to work, but it doesn't
        //if(index < 0) return { unexpect_t{}, errc::invalid_argument };
        if((unsigned)index >= items.size()) return estd::unexpected<errc>(errc::invalid_argument);

        items[index]->action(out);

        return {};
    }

    void render(ostream& out) const
    {
        int i = 0;

        for(const Action* item : items)
        {
            out << ++i << ": ";
            item->render(out);
            out << estd::endl;
        }
    }


    constexpr const char* name() const { return name_; }
};


class MenuAction :  public Action
{
    Navigator* navigator_;
    Menu* menu_;

public:
    constexpr MenuAction(Navigator* navigator, Menu* menu) :
        navigator_{navigator},
        menu_{menu}
    {}

    void render(ostream& out) const override
    {
        out << menu_->name();
    }

    void action(ostream& out) override
    {
        out << menu_->name() << ": " << estd::endl << estd::endl;
        out << "0. upward" << estd::endl;

        navigator_->current(menu_);
        menu_->render(out);
    }

    Menu* menu() { return menu_; }
};

}