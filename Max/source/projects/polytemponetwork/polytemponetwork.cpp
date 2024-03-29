#include "c74_min.h"
#include "../../../../../Source/Library/Polytempo_LibApi.h"
#include "../../../../../Source/Network/Polytempo_PortDefinition.h"

using namespace c74::min;

class polytemponetwork : public object<polytemponetwork>, public PolytempoEventCallbackHandler, public PolytempoTickCallbackHandler, public PolytempoStateCallbackHandler {
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
        "received polytempo events"
    };
    
    outlet<> tickOutput{
        this,
        "(float) current time", "float"
    };
    
    outlet<thread_check::scheduler, thread_action::fifo> networkStateOutput{
        this,
        "network state information"
    };
    
    outlet<thread_check::scheduler, thread_action::fifo> connectedPeersOutput{
        this,
        "connected peers"
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
        m_Port = (args.size() > 0 ? (int)args[0] : POLYTEMPO_NETWORK_PORT_MAX);
        m_InstanceName = "Untitled";
        
        if (!dummy())
        {
            logDebugMessage("init");
            
            // TODO: might be removed
            if (m_initialized)
            {
                logDebugMessage("deinit");
                polytempo_unregisterEventCallback(this);
                polytempo_unregisterTickCallback(this);
                polytempo_unregisterStateCallback(this);
                polytempo_release();
            }

            if (polytempo_isInitialized() || polytempo_initialize(m_Port, master, "MaxExternal") == 0)
            {
                logDebugMessage("register event callback");
                PolytempoEventCallbackOptions eventOptions;
                eventOptions.ignoreTimeTag = true;
                polytempo_registerEventCallback(this, eventOptions);
                
                logDebugMessage("register tick callback");
                PolytempoTickCallbackOptions tickOptions;
                polytempo_registerTickCallback(this, tickOptions);
                
                logDebugMessage("register state callback");
                PolytempoStateCallbackOptions stateOptions;
                stateOptions.callOnChangeOnly = true;
                polytempo_registerStateCallback(this, stateOptions);

                m_initialized = true;
                setInstanceName();
            }
            else
            {
                logMessage("error initializing polytempo lib");
            }
        }
    }
    
    ~polytemponetwork()
    {
        if (!dummy() && m_initialized)
        {
            logDebugMessage("deinit");
            m_initialized = false;
            polytempo_unregisterEventCallback(this);
            polytempo_unregisterTickCallback(this);
            polytempo_unregisterStateCallback(this);
            polytempo_release();
        }
    }
    
    void logMessage(string msg)
    {
        cout << msg << endl;
    }
    
    inline void logDebugMessage(string msg)
    {
        if(verbose)
            cout << msg << endl;
    }

    void setInstanceName()
    {
        PolytempoEventArgument arg;
        arg.name = "name";
        arg.eValueType = PolytempoEventArgument::Value_String;
        arg.valueString = m_InstanceName;
        
        PolytempoEventDto dto;
        dto.command = "settings";
        dto.arguments.push_back(arg);
        polytempo_sendEvent(dto);
    }

    void setMaster(bool isMaster)
    {
        if (m_initialized)
        {
            if (isMaster)
            {
                logDebugMessage("Switch master ON");
                polytempo_toggleMaster(true);

            }
            else
            {
                logDebugMessage("Switch master OFF");
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
    
    attribute<bool> verbose{ this, "verbose", false,
        description {"Turn on/off verbose mode."}
    };
    
    // respond to the bang message to do something
    message<> bang { this, "bang", "Bang, currently unused",
        MIN_FUNCTION {
            return {};
        }
    };

    PolytempoEventArgument::EValueType atomTypeToArgType(atom a)
    {
        if(a.type() == message_type::int_argument)
            return PolytempoEventArgument::Value_Int;
        if(a.type() == message_type::float_argument)
            return PolytempoEventArgument::Value_Double;
        return PolytempoEventArgument::Value_String;
    }
    
    PolytempoEventDto buildEventDto(std::string cmd, const c74::min::atoms& args)
    {
        PolytempoEventDto dto;
        int index = 0;
        
        if(cmd.empty())
        {
            dto.command = (std::string)args[0];
            index++;
        }
        else
        {
            dto.command = cmd;
        }
        
        if(args.size() == index + 1) // means only one argument
        {
            PolytempoEventArgument a;
            a.name = "";
            a.valueString = (std::string)args[index];
            a.eValueType = atomTypeToArgType(args[index]);
            dto.arguments.push_back(a);
            return dto;
        }
        
        while(index < args.size())
        {
            PolytempoEventArgument a;
            a.name = (std::string)args[index];
            index++;
            if(index < args.size())
            {
                a.valueString = (std::string)args[index];
                a.eValueType = atomTypeToArgType(args[index]);
                dto.arguments.push_back(a);
            }
            else
            {
                logMessage("invalid command");
            }
        }
        
        return dto;
    }
    
    message<threadsafe::yes> name { this, "settings", "Change instance name",
        MIN_FUNCTION {
            if (m_initialized)
            {
                if (args[0] == "name" && args.size() > 1)
                {
                    m_InstanceName = (string)args[1];
                    setInstanceName();
                }
                else
                {
                    PolytempoEventDto dto = buildEventDto("settings", args);
                    if(polytempo_sendEvent(dto) != 0)
                    {
                        logMessage("unable to send settings command: " + dto.command);
                    }
                }
            }

            return {};
        }
    };

    message<threadsafe::yes> command { this, "anything", "Generic Events",
        MIN_FUNCTION {
            if (m_initialized && args.size() > 0)
            {
                PolytempoEventDto dto = buildEventDto("", args);
                
                if(polytempo_sendEvent(dto) != 0)
                {
                    logMessage("unable to send event: " + dto.command);
                }
            }

            return {};
        }
    };

    // post to max window == but only when the class is loaded the first time
    message<> maxclass_setup { this, "maxclass_setup",
        MIN_FUNCTION {
            cout << "--- polytempo network ---" << endl;
            cout << "© 2022 Zurich University of the Arts" << endl;
            return {};
        }
    };
    
    void processEvent(PolytempoEventDto const& object) override
    {
        logDebugMessage(object.command);
        
        // split and fill into a list of atoms
        atoms a;
        a.push_back(object.command);
            
        for(auto arg : object.arguments) {
            a.push_back(arg.name);
            switch(arg.eValueType)
            {
            case PolytempoEventArgument::Value_Int:
                a.push_back(std::stoi(arg.valueString));
                break;
            case PolytempoEventArgument::Value_Double:
                a.push_back(std::stod(arg.valueString));
                break;
            case PolytempoEventArgument::Value_String:
                a.push_back(arg.valueString);
                break;
            case PolytempoEventArgument::Value_Int64:
                a.push_back(std::stol(arg.valueString));
                break;
            default:
                logMessage("Polytempo event with unknown argument type");
            }
        }
        
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
            peerAtoms.push_back("-");
        }
        
        connectedPeersOutput.send(peerAtoms);
    }
            
    void processMasterChanged(bool isMaster) override
    {
        if(master != isMaster)
        {
            master = isMaster;
        }
    }
};


MIN_EXTERNAL(polytemponetwork);
