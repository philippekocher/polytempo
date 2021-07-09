#pragma once

#include "JuceHeader.h"
#include "Polytempo_GraphicsViewRegion.h"

#define STANDARD_FONT_SIZE 40.0f
#define STANDARD_LINE_WEIGHT 5.0f

class Polytempo_GraphicsViewRegion;

class Polytempo_GraphicsAnnotation
{
public:
    Polytempo_GraphicsAnnotation();

    Polytempo_GraphicsAnnotation(const Uuid& id, const String& imageId, const Point<float>& referencePoint, const Colour& color, const Path& freeHandPath, const String& text, float fontSize, float lineWeight) : m_id(id),
                                                                                                                                                                                                                       m_imageId(imageId),
                                                                                                                                                                                                                       m_referencePoint(referencePoint),
                                                                                                                                                                                                                       m_color(color),
                                                                                                                                                                                                                       m_freeHandPath(freeHandPath),
                                                                                                                                                                                                                       m_text(text),
                                                                                                                                                                                                                       m_fontSize(fontSize),
                                                                                                                                                                                                                       m_lineWeight(lineWeight),
                                                                                                                                                                                                                       m_displayedImage(nullptr)
    {
    }

    ~Polytempo_GraphicsAnnotation();

    void clear();

    Uuid m_id;
    String m_imageId;
    Point<float> m_referencePoint;
    Colour m_color;
    Path m_freeHandPath;
    String m_text;
    float m_fontSize;
    float m_lineWeight;
    Polytempo_GraphicsViewRegion::displayedImage* m_displayedImage; // temporary usage
};
