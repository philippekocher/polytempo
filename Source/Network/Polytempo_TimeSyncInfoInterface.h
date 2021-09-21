#pragma once

class Polytempo_TimeSyncInfoInterface
{
public:
    virtual ~Polytempo_TimeSyncInfoInterface() = default;
    virtual void showInfoMessage(int messageType, String message) = 0;
};
