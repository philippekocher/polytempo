#pragma once

#include "JuceHeader.h"

#define STANDARD_FONT_SIZE 40.0f
#define STANDARD_LINE_WEIGHT 5.0f

class Polytempo_GraphicsViewRegion;

class Polytempo_GraphicsAnnotation
{
public:
	Polytempo_GraphicsAnnotation();

	Polytempo_GraphicsAnnotation(const Uuid& id, const String& image_id, const Point<float>& reference_point, const Colour& color, const Path& free_hand_path, const String& text, float fontSize, float lineWeight)
		: id(id),
		  imageId(image_id),
		  referencePoint(reference_point),
		  color(color),
		  freeHandPath(free_hand_path),
		  text(text),
		  fontSize(fontSize),
          lineWeight(lineWeight),
		  pRegion(nullptr)
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
    float lineWeight;
	Polytempo_GraphicsViewRegion* pRegion;	// temporary usage
};
