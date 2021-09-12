#pragma once
#include <JuceHeader.h>

#include "../Network/Polytempo_OSCListener.h"

class Polytempo_LibMain : public ReferenceCountedObject
{
private:
    Polytempo_LibMain();

public:
    virtual ~Polytempo_LibMain();

    int initialize(int port, bool masterFlag);
    int toggleMaster(bool masterFlag);
    int sendEvent(std::string command, std::string payload, std::string destinationNamePattern = "*");

    typedef ReferenceCountedObjectPtr<Polytempo_LibMain> Ptr;
    static Ptr current();
    static void release();

private:
    static Ptr current_;
    std::unique_ptr<Polytempo_OSCListener> oscListener;
    bool isInit;
};
