#include <iostream>
#include <conio.h>
#include "../../Source/Library/Polytempo_LibApi.h"

class MyHandler : public EventCallbackHandler, public TickCallbackHandler, public StateCallbackHandler
{
public:
    void processEvent(std::string const& message) override;
    void processTick(double tick) override;
    void processState(int state, std::string message) override;
};

void MyHandler::processEvent(std::string const& message)
{
    std::cout << "Event: " << message << std::endl;
}

void MyHandler::processTick(double tick)
{
    std::cout << "Tick: " << tick << std::endl;
}

void MyHandler::processState(int state, std::string message)
{
    std::cout << "State: " << state << ": " << message << std::endl;
}

int main()
{
    MyHandler handler;

    try {
        std::cout << "Hello World!" << std::endl;
        polytempo_initialize(47522, false);
        polytempo_registerEventCallback(&handler);
        polytempo_registerStateCallback(&handler);
        polytempo_registerTickCallback(&handler);
        polytempo_sendEvent("settings name Console");
        std::cout << "Init complete!" << std::endl;

        char c;
        std::cout << "press esc to exit! " << std::endl;
        while (true)
        {
            c = _getch();
            if (c == 27)
                break;
        }
    }
    catch (...)
    {
    }

    polytempo_release();
}