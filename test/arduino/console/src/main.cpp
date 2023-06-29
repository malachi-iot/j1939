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


class MenuItem
{
protected:
    typedef arduino_ostream ostream;

public:
    virtual void render(ostream&) const = 0;
    virtual void action() = 0;
};


class Menu;
class SubMenuItem;

class MenuNavigator
{
    friend class Menu;
    friend class SubMenuItem;

    Menu* top_;
    Menu* current_;

public:
    MenuNavigator(Menu* top) :
        top_{top},
        current_{top}
    {}

    Menu* current() const { return current_; }
    bool up();
};


class Menu
{
public:
    typedef arduino_ostream ostream;

    Menu* parent_ = nullptr;
    std::vector<MenuItem*> items;

    Menu() = default;
    explicit Menu(Menu* parent) : parent_{parent} {}

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

    void render(ostream& out)
    {
        int i = 0;

        for(const MenuItem* item : items)
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



class SyntheticMenuItem : public MenuItem
{
    const int delineator_;

public:
    constexpr SyntheticMenuItem(int delineator) :
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


class SubMenuItem :  public MenuItem
{
    MenuNavigator* navigator_;
    const char* name_;
    Menu* menu_;

public:
    constexpr SubMenuItem(MenuNavigator* navigator, const char* name, Menu* menu = nullptr) :
        navigator_{navigator},
        name_{name},
        menu_{menu}
    {}

    void render(ostream& out) const override
    {
        out << name_;
    }

    void action() override
    {
        cout << name_ << ": " << endl << endl;
        cout << "0. upward" << endl;

        navigator_->current_ = menu_;
        menu_->render(cout);
    }
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

Menu topLevel, submenu(&topLevel);
SyntheticMenuItem item1{0}, item2{1};
MenuNavigator nav(&topLevel);
SubMenuItem subitem1(&nav, "submenu#1", &submenu);

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
