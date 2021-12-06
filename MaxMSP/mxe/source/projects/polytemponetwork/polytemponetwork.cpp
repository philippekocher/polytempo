#include "c74_min.h"
#include "../../../../../Source/Library/Polytempo_LibApi.h"
#include "../../../../../Source/Network/Polytempo_PortDefinition.h"

using namespace c74::min;

class polytemponetwork : public object<polytemponetwork>, public EventCallbackHandler, public TickCallbackHandler, public StateCallbackHandler {
public:
    MIN_DESCRIPTION	{"Polytempo Network client for Max MSP."};
    MIN_TAGS		{"utilities"};
    MIN_AUTHOR		{"ICST @ ZHdK"};
    MIN_RELATED		{"print"};

    inlet<>  input
    {
        this,
        "master 0/1: toggle master on/off; other input will be interpreted as polytempo event"
    };
    
    outlet<thread_check::scheduler, thread_action::fifo> eventOutput
    {
        this,
        "output of received polytempo events"
    };
    
    outlet<> tickOutput{
        this,
        "(float) tick information", "float"
    };
    
    outlet<thread_check::scheduler, thread_action::fifo> networkStateOutput{
        this,
        "output of network state information"
    };
    
    outlet<thread_check::scheduler, thread_action::fifo> connectedPeersOutput{
        this,
        "output of connected peers"
    };

    timer<> tickDeliverer { this,
        MIN_FUNCTION {
            tickOutput.send(m_Tick);
            return {};
        }
    };
    
    string m_InstanceName;
    bool m_initialized { false };
    int m_Port;
    double m_Tick;

    polytemponetwork(const atoms& args = {})
    {
        m_Port = (args.size() > 0 ? (int)args[0] : Polytempo_PortDefinition::PolytempoNetworkMax);
        m_InstanceName = "MaxExternal";
        
        if (!dummy())
        {
            cout << "init" << endl;
            if (m_initialized)
            {
                cout << "deinit" << endl;
                polytempo_release();
            }

            int ret = polytempo_initialize(m_Port, master);
            if (ret == 0)
            {
                cout << "register event callback" << endl;
                polytempo_registerEventCallback(this);
                cout << "register tick callback" << endl;
                polytempo_registerTickCallback(this);
                cout << "register state callback" << endl;
                polytempo_registerStateCallback(this);

                m_initialized = true;
                setInstanceName();
            }
            else
            {
                cout << "error initializing polytempo lib" << endl;
            }
        }
    }
    
    ~polytemponetwork()
    {
        if (!dummy() && m_initialized)
        {
            cout << "deinit" << endl;
            m_initialized = false;
            polytempo_release();
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

    void setMaster(bool isMaster)
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

    attribute<bool> master{ this, "master", false,
        description {"Turn on/off master."},
        setter { MIN_FUNCTION {
            setMaster(args[0]);
            return args;
        }}
    };
    
    // respond to the bang message to do something
    message<> bang { this, "bang", "Bang, currently unused",
        MIN_FUNCTION {
            return {};
        }
    };

    message<threadsafe::yes> name { this, "settings", "Change instance name",
        MIN_FUNCTION {
            if (m_initialized)
            {
                if (args[0] == "name")
                {
                    m_InstanceName = (string)args[1];
                    setInstanceName();
                }
                else
                {
                    string message = "settings";
                    for(auto m : args)
                    {
                        string addMessage = m;
                        message = message + " " + addMessage;
                    }

                    if(polytempo_sendEvent(message) != 0)
                    {
                        cout << "unable to send settings command: " << message << endl;
                    }
                }
            }

            return {};
        }
    };

    message<threadsafe::yes> command { this, "anything", "Generic Events",
        MIN_FUNCTION {
            if (m_initialized)
            {
                std::string command;
                        
                for(int i = 0; i < args.size(); i++)
                {
                    std::string argString = args[i];
                    command = command + argString + ((i < args.size() - 1) ? " " : "");
                }
                
                if(polytempo_sendEvent(command) != 0)
                {
                    cout << "unable to send event: " << command << endl;
                }
            }

            return {};
        }
    };

    // post to max window == but only when the class is loaded the first time
    message<> maxclass_setup { this, "maxclass_setup",
        MIN_FUNCTION {
            return {};
        }
    };
         
    void processEvent(std::string const& message) override
    {
        std::cout << "Event: " << message << std::endl;
        logMessage(message);
        
        // split and fill into a list of atoms
        atoms a;
        std::istringstream iss(message);
        std::vector<std::string> results(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
        for(auto s : results)
            a.push_back(s);
        
        eventOutput.send(a);
    }

    void processTick(double tick) override
    {
        m_Tick = tick;
        tickDeliverer.delay(0);
    }

    void processState(int state, std::string message, std::vector<std::string> peers) override
    {
        atoms a;
        a.push_back(state);
        a.push_back(message);
        networkStateOutput.send(a);

        atoms peerAtoms;
        for(auto p : peers)
        {
            peerAtoms.push_back(p);
        }

        if(peers.size() <= 0)
        {
            peerAtoms.push_back(0);
        }
                            
        connectedPeersOutput.send(peerAtoms);
    }
};


MIN_EXTERNAL(polytemponetwork);
