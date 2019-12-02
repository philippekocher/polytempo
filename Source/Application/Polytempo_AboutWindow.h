#ifndef __Polytempo_AboutWindow__
#define __Polytempo_AboutWindow__

#include "JuceHeader.h"

class Polytempo_AboutWindow: public Component
{
public:
    Polytempo_AboutWindow();
    ~Polytempo_AboutWindow();

    static void show();
    void closeButtonPressed();
    
private:
    std::unique_ptr<Label> label1;
	std::unique_ptr<Label> label2;
	std::unique_ptr<Label> label3;
	std::unique_ptr<Label> label4;
	std::unique_ptr<ImageComponent> iconComponent;
	std::unique_ptr<HyperlinkButton> link1;
	std::unique_ptr<HyperlinkButton> link2;
	std::unique_ptr<Component> aboutComponent;
};


#endif  // __Polytempo_AboutWindow__
