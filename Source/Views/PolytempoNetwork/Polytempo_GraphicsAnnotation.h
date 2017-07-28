/*
  ==============================================================================

    Polytempo_GraphicsAnnotation.h
    Created: 28 Jul 2017 5:26:47am
    Author:  chris

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class Polytempo_GraphicsAnnotation
{
public:
	Polytempo_GraphicsAnnotation();
	void clear();

	String imageId;
	Point<float> referencePoint;
	Colour color;
	Path freeHandPath;
	String text;
};