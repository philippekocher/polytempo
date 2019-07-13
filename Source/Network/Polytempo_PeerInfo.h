/*
  ==============================================================================

    Polytempo_PeerInfo.h
    Created: 13 Apr 2019 3:57:42pm
    Author:  christian.schweizer

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class Polytempo_PeerInfo
{
public:
	String	ip;
	String	scoreName;
	String	peerName;
	uint32	lastHeartBeat;
	bool	syncState;
};