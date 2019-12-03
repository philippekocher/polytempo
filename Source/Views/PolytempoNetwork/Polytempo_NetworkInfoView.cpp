#include "JuceHeader.h"
#include "Polytempo_NetworkInfoView.h"
#include "../../Network/Polytempo_NetworkSupervisor.h"
#include "../../Network/Polytempo_TimeProvider.h"

Polytempo_NetworkInfoView::Polytempo_NetworkInfoView()
{
}

Polytempo_NetworkInfoView::~Polytempo_NetworkInfoView()
{
}

void Polytempo_NetworkInfoView::paint(Graphics& g)
{
    AttributedString attributedPeers;
    attributedPeers.append("Local:\n", Font(12.0f, Font::bold));
    attributedPeers.append(" \n", Font(4.0f, Font::plain));
    attributedPeers.append(Polytempo_NetworkSupervisor::getInstance()->getDescription() + "\n", Font(12.0f, Font::plain));
    attributedPeers.append(" \n", Font(12.0f, Font::plain));

    if (Polytempo_TimeProvider::getInstance()->isMaster())
    {
        OwnedArray<Polytempo_PeerInfo> peers;
        Polytempo_InterprocessCommunication::getInstance()->getClientsInfo(&peers);

        if (peers.size() == 0)
        {
            attributedPeers.append("No connected peers", Font(12, Font::plain), Colours::orangered);
        }
        else
        {
            attributedPeers.append("Connected peers:\n", Font(12, Font::bold));
            for (Polytempo_PeerInfo* peer : peers)
            {
                attributedPeers.append(" \n", Font(4.0f, Font::plain));
                attributedPeers.append(peer->scoreName + " (" + peer->peerName + ")\n", Font(12.0f, Font::plain), peer->connectionOk ? Colours::darkgreen : Colours::orangered);
            }
        }
    }
    else
    {
        std::unique_ptr<Polytempo_PeerInfo> peer = std::unique_ptr<Polytempo_PeerInfo>(Polytempo_InterprocessCommunication::getInstance()->getMasterInfo());

        if (peer != nullptr)
        {
            attributedPeers.append("Connected to Master:\n", Font(12, Font::bold));
            attributedPeers.append(" \n", Font(4.0f, Font::plain));
            attributedPeers.append(peer->scoreName + " (" + peer->peerName + ")\n", Font(12.0f, Font::plain), peer->connectionOk ? Colours::darkgreen : Colours::orangered);
        }
    }
    attributedPeers.draw(g, Rectangle<int>(0, 10, getWidth(), getHeight()).toFloat());
}

void Polytempo_NetworkInfoView::resized()
{
    Polytempo_NetworkSupervisor::getInstance()->setComponent(this);
}
