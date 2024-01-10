#include "Polytempo_GraphicsAnnotation.h"

Polytempo_GraphicsAnnotation::Polytempo_GraphicsAnnotation()
{
    clear();
}

void Polytempo_GraphicsAnnotation::clear()
{
    m_color = Colours::black;
    m_freeHandPath.clear();
    m_imageId = String();
    m_referencePoint = Point<float>();
    m_text = String();
    m_fontSize = STANDARD_FONT_SIZE;
}
