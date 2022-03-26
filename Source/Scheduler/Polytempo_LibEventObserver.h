/*
  ==============================================================================

    Polytempo_LibEventObserver.h
    Created: 15 Sep 2021 1:14:04pm
    Author:  chris

  ==============================================================================
*/

#pragma once
#include "Polytempo_EventObserver.h"

class Polytempo_LibEventObserver : public Polytempo_EventObserver, public ActionBroadcaster
{
public:
    Polytempo_LibEventObserver(ActionListener* listener) { addActionListener(listener); }
    void eventNotification(Polytempo_Event* event) override;
};
