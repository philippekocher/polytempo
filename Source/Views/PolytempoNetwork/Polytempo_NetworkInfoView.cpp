/*
  ==============================================================================

    Polytempo_NetworkInfoView.cpp
    Created: 5 Dec 2017 2:41:16pm
    Author:  christian.schweizer

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Polytempo_NetworkInfoView.h"
#include "../../Network/Polytempo_NetworkSupervisor.h"
#include "../../Network/Polytempo_TimeProvider.h"

//==============================================================================
Polytempo_NetworkInfoView::Polytempo_NetworkInfoView()
{
}

Polytempo_NetworkInfoView::~Polytempo_NetworkInfoView()
{
}

void Polytempo_NetworkInfoView::paint (Graphics& g)
{
	AttributedString attributedPeers;
    attributedPeers.append("Local:\n", Font(12.0f, Font::bold));
    attributedPeers.append(" \n", Font(4.0f, Font::plain));
    attributedPeers.append(Polytempo_NetworkSupervisor::getInstance()->getDescription() + "\n", Font(12.0f, Font::plain));
	attributedPeers.append(" \n", Font(12.0f, Font::plain));
	HashMap<Uuid, Polytempo_PeerInfo>* peersMap = Polytempo_NetworkSupervisor::getInstance()->getPeers();
	uint32 currentTime = Time::getMillisecondCounter();

	if(!Polytempo_TimeProvider::getInstance()->isMaster())
	{
		if(peersMap->size() == 1)
		{
			Polytempo_PeerInfo peerInfo = peersMap->begin().getValue();
			attributedPeers.append("Connected to Master:\n", Font(12, Font::bold));
			Colour peerColor = (currentTime - peerInfo.lastHeartBeat) > 2 *  TIME_SYNC_INTERVAL_MS
				? Colours::red
				: Colours::green;
			attributedPeers.append(" \n", Font(4.0f, Font::plain));
			attributedPeers.append(peerInfo.scoreName + " (" + peerInfo.peerName + ")\n", Font(12.0f, Font::plain), peerColor);
		}
	}
	else
	{
		attributedPeers.append("Connected peers:\n", Font(12, Font::bold));
		HashMap < Uuid, Polytempo_PeerInfo >::Iterator it(*peersMap);
		while (it.next())
		{
			Colour peerColor = it.getValue().syncState
				? Colours::green
				: Colours::orange;
			attributedPeers.append(" \n", Font(4.0f, Font::plain));
			attributedPeers.append(it.getValue().scoreName + " (" + it.getValue().peerName + ")\n", Font(12.0f, Font::plain), peerColor);
		}
	}
	attributedPeers.draw(g, Rectangle<int>(0, 10, getWidth(), getHeight()).toFloat());
}

void Polytempo_NetworkInfoView::resized()
{
 	Polytempo_NetworkSupervisor::getInstance()->setComponent(this);
}
