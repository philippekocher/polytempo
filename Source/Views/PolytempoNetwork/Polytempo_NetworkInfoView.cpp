/*
  ==============================================================================

    Polytempo_NetworkInfoView.cpp
    Created: 5 Dec 2017 2:41:16pm
    Author:  chris

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
    attributedPeers.append("Network:\n", Font(12.0f, Font::bold));
	attributedPeers.append(" \n", Font(4.0f, Font::plain));
	attributedPeers.append(Polytempo_NetworkSupervisor::getInstance()->getAdapterInfo() + "\n\n", Font(12.0f, Font::plain));
    attributedPeers.append("Local:\n", Font(12.0f, Font::bold));
    attributedPeers.append(" \n", Font(4.0f, Font::plain));
    attributedPeers.append(Polytempo_NetworkSupervisor::getInstance()->getLocalName() + "\n", Font(12.0f, Font::plain));
	attributedPeers.append(" \n", Font(12.0f, Font::plain));
	attributedPeers.append("Connected peers:\n", Font(12, Font::bold));
	HashMap < String, String >::Iterator it(*Polytempo_NetworkSupervisor::getInstance()->getPeers());
	while (it.next())
	{
		attributedPeers.append(" \n", Font(4.0f, Font::plain));
		attributedPeers.append(it.getValue() + "\n", Font(12.0f, Font::plain));
	}

	attributedPeers.draw(g, Rectangle<int>(0, 10, getWidth(), getHeight()).toFloat());
}

void Polytempo_NetworkInfoView::resized()
{
 	Polytempo_NetworkSupervisor::getInstance()->setComponent(this);
}
