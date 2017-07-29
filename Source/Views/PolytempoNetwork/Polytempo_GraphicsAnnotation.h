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

	Polytempo_GraphicsAnnotation(const String& image_id, const Point<float>& reference_point, const Colour& color, const Path& free_hand_path, const String& text)
		: imageId(image_id),
		  referencePoint(reference_point),
		  color(color),
		  freeHandPath(free_hand_path),
		  text(text)
	{
	}

	~Polytempo_GraphicsAnnotation();

	void clear();

	String imageId;
	Point<float> referencePoint;
	Colour color;
	Path freeHandPath;
	String text;
};