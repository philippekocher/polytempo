#include "c74_min.h"
#include "../../../../../Source/Library/Polytempo_LibApi.h"

using namespace c74::min;

class polytemponetwork;

class MyHandler : public EventCallbackHandler, public TickCallbackHandler, public StateCallbackHandler
{
public:
    MyHandler(polytemponetwork* pExternal);
    void processEvent(std::string const& message) override;
    void processTick(double tick) override;
    void processState(int state, std::string message) override;
    
private:
    polytemponetwork* m_pExternal;
};

class polytemponetwork : public object<polytemponetwork> {
public:
    MIN_DESCRIPTION	{"Polytempo Network client for Max MSP."};
    MIN_TAGS		{"utilities"};
    MIN_AUTHOR		{"ICST @ ZHdK"};
    MIN_RELATED		{"print"};

    inlet<>  input	{ this, "(int) toggle client on/off\n(master) toggle master on/off" };
    outlet<thread_check::scheduler, thread_action::fifo> eventOutput	{ this, "(anything) output of received polytempo events" };
    outlet<thread_check::scheduler, thread_action::fifo> tickOutput{ this, "(float) tick information" };
    outlet<thread_check::scheduler, thread_action::fifo> networkStateOutput{this, "(anything) output of network state information" };

    MyHandler* pHandler;
    string m_InstanceName;
    bool m_initialized { false };
    int m_Port;

    polytemponetwork(const atoms& args = {})
    {
        m_Port = args.size() > 0 ? args[0] : 47522;
        m_InstanceName = "MaxExternal";
    }
    
    ~polytemponetwork()
    {
        if (!dummy())
        {
            setActive(false);
        }
    }
    
    void logMessage(string msg)
    {
        cout << msg << endl;
    }

    void setInstanceName()
    {
        polytempo_sendEvent("settings name " + m_InstanceName);
    }

    void setActive(bool on)
    {
        if(on)
        {
            cout << "init" << endl;
            if (m_initialized)
            {
                cout << "deinit" << endl;
                polytempo_release();
            }

            int ret = polytempo_initialize(m_Port, isMaster);
            if (ret == 0)
            {
                pHandler = new MyHandler(this);

                cout << "register event callback" << endl;
                polytempo_registerEventCallback(pHandler);
                cout << "register tick callback" << endl;
                polytempo_registerTickCallback(pHandler);
                cout << "register state callback" << endl;
                polytempo_registerStateCallback(pHandler);

                m_initialized = true;
                setInstanceName();
            }
            else
            {
                cout << "error initializing polytempo lib" << endl;
            }
        }
        else 
        {
            if (m_initialized)
            {
                cout << "deinit" << endl;
                m_initialized = false;
                polytempo_release();
            }
        }
    }

    void setMaster()
    {
        if (m_initialized)
        {
            if (isMaster)
            {
                cout << "Switch master ON" << endl;
                polytempo_toggleMaster(true);

            }
            else
            {
                cout << "Switch master OFF" << endl;
                polytempo_toggleMaster(false);

            }
        }
    }

    attribute<bool> on{ this, "on", false,
        description {"Turn on/off the network client."},
        setter { MIN_FUNCTION {
            setActive(args[0] == true);
            return args;
        }}
    };

    attribute<bool> isMaster{ this, "isMaster", false,
        description {"Turn on/off master."},
        setter { MIN_FUNCTION {
            return args;
        }}
    };

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

            //polytempo_sendEvent("start");
            return {};
        }
    };

    message<> number{ this, "int", "On/off toggle",
        MIN_FUNCTION {
            on = args[0] == 1;
            return {};
        }
    };

    message<threadsafe::yes> masterSwitch { this, "master", "Master switch",
        MIN_FUNCTION {
            if (m_initialized)
            {
                isMaster = (args[0] == 1);
                setMaster();
            }

            return {};
        }
    };

    message<threadsafe::yes> name { this, "settings", "Change instance name",
        MIN_FUNCTION {
            if (m_initialized)
            {
                if (args[0] == "name")
                {
                    m_InstanceName = args[1];
                    setInstanceName();
                }
            }

            return {};
        }
    };

    message<threadsafe::yes> command { this, "event", "Generic Events",
        MIN_FUNCTION {
            if (m_initialized)
            {
                if(polytempo_sendEvent(args[0]) != 0)
                {
                    cout << "unable to send event: " << args[0] << endl;
                }
            }

            return {};
        }
    };

    // post to max window == but only when the class is loaded the first time
    message<> maxclass_setup { this, "maxclass_setup",
        MIN_FUNCTION {

            if (!dummy() && on)
            {
                setActive(true);
                setMaster();
            }

            return {};
        }
    };
            
};

MyHandler::MyHandler(polytemponetwork* pExternal)
{
    m_pExternal = pExternal;
}

void MyHandler::processEvent(std::string const& message)
{
    std::cout << "Event: " << message << std::endl;
    m_pExternal->logMessage(message);
    m_pExternal->eventOutput.send(message);
}

void MyHandler::processTick(double tick)
{
    std::cout << "Tick: " << tick << std::endl;
    m_pExternal->logMessage("Tick: " + std::to_string(tick));
    m_pExternal->tickOutput.send(std::to_string(tick));
}

void MyHandler::processState(int state, std::string message)
{
    std::cout << "State: " << state << ": " << message << std::endl;
    string msg = "State: " + std::to_string(state) + " " + message;
    m_pExternal->networkStateOutput.send(msg);
}


MIN_EXTERNAL(polytemponetwork);
