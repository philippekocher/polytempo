/*
  ==============================================================================

    Polytempo_GraphicsAnnotation.cpp
    Created: 28 Jul 2017 5:26:47am
    Author:  chris

  ==============================================================================
*/

#include "Polytempo_GraphicsAnnotation.h"

Polytempo_GraphicsAnnotation::Polytempo_GraphicsAnnotation()
{
	clear();
}

Polytempo_GraphicsAnnotation::~Polytempo_GraphicsAnnotation()
{
	clear();
}

void Polytempo_GraphicsAnnotation::clear()
{
	color = Colours::black;
	freeHandPath.clear();
	imageId = String::empty;
	referencePoint = Point<float>();
	text = String::empty;
	fontSize = STANDARD_FONT_SIZE;
}
