/*
  ==============================================================================

    Polytempo_GraphicsAnnotation.h
    Created: 28 Jul 2017 5:26:47am
    Author:  chris

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#define STANDARD_FONT_SIZE 16.0f

class Polytempo_GraphicsAnnotation
{
public:
	Polytempo_GraphicsAnnotation();

	Polytempo_GraphicsAnnotation(const Uuid& id, const String& image_id, const Point<float>& reference_point, const Colour& color, const Path& free_hand_path, const String& text, float fontSize)
		: id(id),
		  imageId(image_id),
		  referencePoint(reference_point),
		  color(color),
		  freeHandPath(free_hand_path),
		  text(text), 
	      fontSize(fontSize)
	{
	}

	~Polytempo_GraphicsAnnotation();

	void clear();

	Uuid id;
	String imageId;
	Point<float> referencePoint;
	Colour color;
	Path freeHandPath;
	String text;
	float fontSize;
};