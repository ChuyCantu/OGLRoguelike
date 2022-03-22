#pragma once

#include "Common.hpp"
#include "Utils/Color.hpp"
#include "Widget.hpp"
#include "Text/TextRenderer.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>



class Label : public Widget {
public:
    Label(const std::string& text, int textSize, const std::string& name = "Label");
    Label(const std::string& text, int textSize, const Rect& rect, const std::string& name = "Label");
    Label(const std::string& text, int textSize, const glm::vec2& size, const std::string& name = "Label");

    void Draw() override;

    void SetText(const std::string& text);
    void SetFont(const Font& font);
    void SetTextSize(int size);
    void SetLetterSpacing(float value);
    void SetLineSpacing(float value);
    void SetWordSpacing(float value);
    void SetSpacesPerTab(int value);

    const std::string& GetText() const { return text; };
    const Font& GetFont() const { return font; }
    int GetTextSize() const { return textSize; }
    float GetLetterSpacing() const { return settings.letterSpacing; }
    float GetLineSpacing() const { return settings.lineSpacing; }
    float GetWordSpacing() const { return settings.wordSpacing; }
    int GetSpacesPerTab() const { return settings.spacesPerTab; }
    const glm::vec2& GetTextBounds() const { return textBounds; }
    const std::vector<LineInfo>& GetLinesInfo() const { return textLines; }

private:
    void UpdateLinesAndBounds();

public:
    TextAppearance appearance;
    TextHorzAlign horizontalAlign {TextHorzAlign::Left};
    TextVertAlign verticalAlign   {TextVertAlign::Top};
    TextTransform transform       {TextTransform::None};
    bool wrap                     {false}; // TODO
    bool clipText                 {false};
    
protected:
    std::string text;
    int textSize;

    Font font /*{"SourceCode", 22, FontRenderMode::Raster};*/ {"KenneyPixel", 22, FontRenderMode::SDF};
    TextSettings settings;

    glm::vec2 textBounds;
    std::vector<LineInfo> textLines;
};