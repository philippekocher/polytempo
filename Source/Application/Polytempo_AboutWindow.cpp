/* ==============================================================================
 
 This file is part of the POLYTEMPO software package.
 Copyright (c) 2016 - Zurich University of the Arts,
 ICST Institute for Computer Music and Sound Technology
 http://www.icst.net
 
 Author: Philippe Kocher
 
 POLYTEMPO is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 POLYTEMPO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this software. If not, see <http://www.gnu.org/licenses/>.
 
 ============================================================================== */


#include "Polytempo_AboutWindow.h"

#define _BUILD_YEAR_ \
( \
(__DATE__[ 7] - '0') * 1000 + \
(__DATE__[ 8] - '0') *  100 + \
(__DATE__[ 9] - '0') *   10 + \
(__DATE__[10] - '0') \
)

Polytempo_AboutWindow::Polytempo_AboutWindow()
: DialogWindow ("About", Colour::greyLevel (0.99f), true)
{
    Rectangle<int> rect(0, 0, 700, 200);
    
    String text1, text2, text3, text4;
    
    text1 << JUCEApplication::getInstance()->getApplicationName() << newLine;
    text2 << "Version: " << JUCEApplication::getInstance()->getApplicationVersion();
    text3 << "Build Date: " << __DATE__ << " " << __TIME__;
    text4 << "Copyright (c) "<< _BUILD_YEAR_ << " Zurich University of the Arts";
    
    label1 = new Label(String::empty, text1);
    label1->setFont(Font(76.0f, Font::plain));
    label1->setColour(Label::textColourId, Colours::darkslateblue);
    label1->setJustificationType(Justification::topLeft);
    label1->setBounds(rect);
    
    label2 = new Label(String::empty, text2);
    label2->setFont(Font(16.0f, Font::bold));
    label2->setJustificationType(Justification::topLeft);
    label2->setBounds(4,85,rect.getWidth(),20);
    
    label3 = new Label(String::empty, text3);
    label3->setFont(Font(11.0f, Font::plain));
    label3->setJustificationType(Justification::topLeft);
    label3->setBounds(4,105,rect.getWidth(),20);
    
    label4 = new Label(String::empty, text4);
    label4->setFont(Font(12.0f, Font::plain));
    label3->setJustificationType(Justification::topLeft);
    label4->setBounds(4,rect.getHeight()-20,rect.getWidth(),20);
    
    link = new HyperlinkButton ("ICST Institute for Computermusic and Sound Technology", URL ("http://www.icst.net"));
    link->setFont(Font (12.0f, Font::bold | Font::underlined), false);
    link->setBounds(4,120,0, 20);
    link->changeWidthToFitText();
    
    Component *aboutComponent = new Component();
    aboutComponent->setBounds(rect);
    aboutComponent->addAndMakeVisible(label1);
    aboutComponent->addAndMakeVisible(label2);
    aboutComponent->addAndMakeVisible(link);
    aboutComponent->addAndMakeVisible(label3);
    aboutComponent->addAndMakeVisible(label4);
    
    setContentOwned(aboutComponent, false);
    
    setUsingNativeTitleBar(true);
    setBounds(rect);
    centreWithSize(rect.getWidth(), rect.getHeight());
}

Polytempo_AboutWindow::~Polytempo_AboutWindow()
{
    label1 = nullptr;
    label2 = nullptr;
    link = nullptr;
}

void Polytempo_AboutWindow::show()
{
    Polytempo_AboutWindow window;
    window.runModalLoop();
}

void Polytempo_AboutWindow::closeButtonPressed()
{
    setVisible (false);
}

