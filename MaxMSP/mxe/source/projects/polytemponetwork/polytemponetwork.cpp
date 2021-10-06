//#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED
#include "c74_min.h"
#include "../../../../../Source/Library/Polytempo_LibApi.h"

using namespace c74::min;

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

class polytemponetwork : public object<polytemponetwork> {
public:
    MIN_DESCRIPTION	{"Post to the Max Console."};
    MIN_TAGS		{"utilities"};
    MIN_AUTHOR		{"Cycling '74"};
    MIN_RELATED		{"print, jit.print, dict.print"};

    inlet<>  input	{ this, "(bang) post greeting to the max console" };
    outlet<> output	{ this, "(anything) output the message which is posted to the max console" };
    MyHandler handler;

    // define an optional argument for setting the message
    argument<symbol> greeting_arg { this, "greeting", "Initial value for the greeting attribute.",
        MIN_ARGUMENT_FUNCTION {
            greeting = arg;
        }
    };


    // the actual attribute for the message
    attribute<symbol> greeting { this, "greeting", "hello world",
        description {
            "Greeting to be posted. "
            "The greeting will be posted to the Max console when a bang is received."
            
        }
    };

    
    // respond to the bang message to do something
    message<> bang { this, "bang", "Post the greeting.",
        MIN_FUNCTION {
            symbol the_greeting = greeting;    // fetch the symbol itself from the attribute named greeting

            cout << the_greeting << endl;    // post to the max console
            output.send(the_greeting);       // send out our outlet
            polytempo_sendEvent("start");
            return {};
        }
    };

    // post to max window == but only when the class is loaded the first time
    message<> maxclass_setup { this, "maxclass_setup",
        MIN_FUNCTION {
            cout << "init" << endl;
            polytempo_initialize(47522, false);
            //cout << "register event callback" << endl;
            //polytempo_registerEventCallback(&handler);
            //cout << "register state callback" << endl;
            //polytempo_registerStateCallback(this);
            //cout << "register tick callback" << endl;
            //polytempo_registerTickCallback(&handler);
            polytempo_sendEvent("settings name MaxExternal");
            return {};
        }
    };
};


MIN_EXTERNAL(polytemponetwork);
