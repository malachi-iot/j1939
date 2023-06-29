#include <Arduino.h>

#include <estd/istream.h>
#include <estd/ostream.h>
#include <estd/string.h>

// DEBT: Not available on AVR
#include <vector>
#include <estd/expected.h>

uint32_t start_ms;

using namespace estd;

arduino_ostream cout(Serial);
arduino_istream cin(Serial);

enum class States
{
    Entry,
    Running
};

States state = States::Entry;
layer1::string<128> input;


class MenuAction
{
protected:
    typedef arduino_ostream ostream;

public:
    virtual void render(ostream&) const = 0;
    virtual void action() = 0;
};


class Menu;
class SubMenuAction;

class MenuNavigator
{
    friend class Menu;

    Menu* const top_;
    Menu* current_;

public:
    MenuNavigator(Menu* top) :
        top_{top},
        current_{top}
    {}

    Menu* current() const { return current_; }
    void current(Menu* v) { current_ = v; }
    bool up();
};


class Menu
{
public:
    typedef arduino_ostream ostream;

    std::vector<MenuAction*> items;
    
    Menu* const parent_ = nullptr;
    const char* const name_ = nullptr;

    Menu() = default;
    
    explicit Menu(const char* name, Menu* parent = nullptr) :
        parent_{parent},
        name_{name}
    {}

    expected<void, errc> activate(int index)
    {
        // CTAD not a full option since we're c++11 mostly
        if(index < 0) return unexpected<errc>(errc::invalid_argument);
        // FIX: We expect this brace init list to work, but it doesn't
        //if(index < 0) return { unexpect_t{}, errc::invalid_argument };
        if((unsigned)index >= items.size()) return unexpected<errc>(errc::invalid_argument);

        items[index]->action();

        return {};
    }

    void render(ostream& out) const
    {
        int i = 0;

        for(const MenuAction* item : items)
        {
            out << ++i << ": ";
            item->render(out);
            out << endl;
        }
    }
};



bool MenuNavigator::up()
{
    if(current_->parent_ == nullptr) return false;

    current_ = current_->parent_;
    return true;
}



class SyntheticMenuAction : public MenuAction
{
    const int delineator_;

public:
    constexpr SyntheticMenuAction(int delineator) :
        delineator_{delineator}
    {}

    void render(ostream& out) const override
    {
        out << "synthetic#" << delineator_;
    }

    void action() override
    {
        cout << "activated ";
        render(cout);
    }
};


class UpMenuAction : public MenuAction
{
    MenuNavigator* navigator_;

public:
    constexpr UpMenuAction(MenuNavigator* navigator) :
        navigator_{navigator}
    {}

    void render(ostream& out) const override
    {
        out << "..";
    }

    void action() override
    {
        navigator_->up();
        navigator_->current()->render(cout);
    }
};


class SubMenuAction :  public MenuAction
{
    MenuNavigator* navigator_;
    Menu* menu_;

public:
    constexpr SubMenuAction(MenuNavigator* navigator, Menu* menu) :
        navigator_{navigator},
        menu_{menu}
    {}

    void render(ostream& out) const override
    {
        out << menu_->name_;
    }

    void action() override
    {
        cout << menu_->name_ << ": " << endl << endl;
        cout << "0. upward" << endl;

        navigator_->current(menu_);
        menu_->render(cout);
    }

    Menu* menu() { return menu_; }
};



void menu1(MenuNavigator* nav)
{
    Menu* menu = nav->current();

    if(state == States::Entry)
    {
        cout << F("Top level: ") << endl << endl;
        state = States::Running;

        menu->render(cout);

        return;
    }

    // DEBT: Use proper int_type
    int c = cin.get();

    // DEBT: Check traits eof
    if(c == -1) return;

    cout.put(c);

    switch(c)
    {
        case 10: break;

        case 13:
        //case 10:
        {
            int selected = -1;

            //cout << endl << F("Acting on: ") << input << endl;

            // DEBT: Somewhere we have a string-optimized version of this...
            from_chars(input.data(), input.data() + input.size(), selected);
            if(selected == 0)
            {
                if(!nav->up())
                    cout << "Topmost menu, cannot go up further";
                else
                    nav->current()->render(cout);
            }
            else if(selected != -1)
            {
                menu->activate(selected - 1);

                /* FIX: something's wrong with copy constructor
                expected<void, errc> r = menu->activate(selected - 1);

                if(r.has_error())
                {

                }   */
            }

            input.clear();
            break;
        }

        default:
            // DEBT: Should we have to cast this?
            input += (char)c;
            break;
    }
}

Menu topLevel, submenu("submenu#1", &topLevel);
SyntheticMenuAction item1{0}, item2{1};
MenuNavigator nav(&topLevel);
SubMenuAction subitem1(&nav, &submenu);

void setup() 
{
    topLevel.items.push_back(&item1);
    topLevel.items.push_back(&item2);
    topLevel.items.push_back(&subitem1);

    Serial.begin(115200);

    delay(1000);
}

void loop() 
{
    uint32_t now_ms = millis();

    long count = now_ms - start_ms;

/*
    cout << F("Please input something: ");

    estd::layer1::string<128> buffer;

    cin >> buffer;
    cout << F("You input: ") << buffer << estd::endl; */
    menu1(&nav);
}
