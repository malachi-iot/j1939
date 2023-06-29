#include <Arduino.h>

#include <estd/istream.h>
#include <estd/ostream.h>
#include <estd/string.h>

// DEBT: Not available on AVR
#include <vector>
#include <estd/expected.h>

#include "menu.h"

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


namespace menu {


bool Navigator::up()
{
    if(current_->parent_ == nullptr) return false;

    current_ = current_->parent_;
    return true;
}

}



class SyntheticMenuAction : public menu::Action
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

    void action(ostream& out) override
    {
        out << "activated ";
        render(out);
    }
};


// EXPERIMENTAL
class UpMenuAction : public menu::Action
{
    menu::Navigator* navigator_;

public:
    constexpr UpMenuAction(menu::Navigator* navigator) :
        navigator_{navigator}
    {}

    void render(ostream& out) const override
    {
        out << "..";
    }

    void action(ostream& out) override
    {
        navigator_->up();
        navigator_->current()->render(out);
    }
};






void menu1(menu::Navigator* nav)
{
    menu::Menu* menu = nav->current();

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
                menu->activate(selected - 1, cout);

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

menu::Menu topLevel, submenu("submenu#1", &topLevel);
SyntheticMenuAction item1{0}, item2{1};
menu::Navigator nav(&topLevel);
menu::MenuAction subitem1(&nav, &submenu);

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
/*
    uint32_t now_ms = millis();

    long count = now_ms - start_ms;

    cout << F("Please input something: ");

    estd::layer1::string<128> buffer;

    cin >> buffer;
    cout << F("You input: ") << buffer << estd::endl; */
    menu1(&nav);
}
