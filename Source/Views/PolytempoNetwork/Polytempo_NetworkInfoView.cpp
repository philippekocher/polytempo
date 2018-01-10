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
	attributedPeers.append("Network\n", Font(14.0f, Font::plain));
	attributedPeers.append(" \n", Font(8.0f, Font::plain));
	attributedPeers.append(Polytempo_NetworkSupervisor::getInstance()->getLocalName() + "\n", Font(12.0f, Font::bold));
	attributedPeers.append(" \n", Font(8.0f, Font::plain));
	attributedPeers.append(Polytempo_NetworkSupervisor::getInstance()->getAdapterInfo() + "\n", Font(12.0f, Font::plain));
	attributedPeers.append(" \n", Font(22.0f, Font::plain));
	attributedPeers.append(Polytempo_TimeProvider::getInstance()->isMaster() ? "Master: yes \n" : "Master: no \n", Font(14.0f, Font::plain));
	attributedPeers.append("Connected peers:\n", Font(12, Font::bold));
	HashMap < String, String >::Iterator it(*Polytempo_NetworkSupervisor::getInstance()->getPeers());
	while (it.next())
	{
		attributedPeers.append(" \n", Font(4.0f, Font::plain));
		attributedPeers.append(it.getValue() + "\n", Font(12.0f, Font::plain));
	}

	g.drawHorizontalLine(0, 0.0f, (float)getWidth());
	attributedPeers.draw(g, Rectangle<int>(10, 10, getWidth()-20, getHeight()).toFloat());

	g.setColour(Colours::grey);
	g.drawVerticalLine(0, 0.0f, (float)getHeight());
}

void Polytempo_NetworkInfoView::resized()
{
 	Polytempo_NetworkSupervisor::getInstance()->setComponent(this);
}
