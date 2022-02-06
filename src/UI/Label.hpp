#ifndef __LABEL_H__
#define __LABEL_H__

#include "Common.hpp"
#include "Utils/Color.hpp"
#include "Widget.hpp"
#include "Text/TextRenderer.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>



class Label : public Widget {
public:
    Label(const std::string& text);
    Label(const std::string& text, const Rect& rect);
    Label(const std::string& text,const glm::vec2& size);

    void Draw() override;

public:
    std::string text;

    Font font                     {"KenneyPixel", 22, FontRenderMode::SDF};
    TextAppearance appearance;
    TextHorzAlign horizontalAlign {TextHorzAlign::Left};
    TextVertAlign verticalAlign   {TextVertAlign::Top};
    TextTransform transform       {TextTransform::None};
    TextSettings settings;

    std::vector<LineInfo> textLines;

    bool clipText {false};
};

#endif // __LABEL_H__