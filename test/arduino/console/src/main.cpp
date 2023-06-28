#include <Arduino.h>

#include <estd/istream.h>
#include <estd/ostream.h>
#include <estd/string.h>

// DEBT: Not available on AVR
#include <vector>
//#include <estd/expected.h>

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


class Menu
{
public:
    Menu* parent = nullptr;
    std::vector<MenuItem*> items;

    estd::expected<void, estd::errc> activate(int index)
    {
        if(index < 0) return;
        if(index > items.size()) return;

        items[index]->action();
    }
};


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



void menu1(Menu* menu)
{
    if(state == States::Entry)
    {
        cout << F("Top level: ") << endl << endl;
        state = States::Running;
        int i = 0;

        for(const MenuItem* item : menu->items)
        {
            cout << ++i << ": ";
            item->render(cout);
            cout << endl;
        }

        return;
    }

    // DEBT: Use proper int_type
    int c = cin.get();

    // DEBT: Check traits eof
    if(c == -1) return;

    cout.put(c);

    switch(c)
    {
        //case '0':

        case 10: break;

        case 13:
        //case 10:
        {
            int selected = -1;

            cout << endl << F("Acting on: ") << input << endl;
            // DEBT: Somewhere we have a string-optimized version of this...
            from_chars(input.data(), input.data() + input.size(), selected);
            if(selected != -1)
                menu->activate(selected - 1);

            input.clear();
            break;
        }

        default:
            // DEBT: Should we have to cast this?
            input += (char)c;
            break;

    }
}

Menu topLevel;
SyntheticMenuItem item1{0}, item2{1};

void setup() 
{
    topLevel.items.push_back(&item1);
    topLevel.items.push_back(&item2);

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
    menu1(&topLevel);
}
