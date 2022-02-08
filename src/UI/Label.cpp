#include "Label.hpp"

#include "Core/Log.hpp"
#include "Rendering/Camera.hpp"
#include "Text/TextRenderer.hpp"

#include <glad/glad.h>

Label::Label(const std::string& text, int textSize) : Widget{Rect{glm::vec2{0.0f}, glm::vec2{0.0f}}}, text{text}, textSize{textSize} {
    SplitTextLines(text, textLines);
    UpdateLinesAndBounds();
    // Actual size is computed by the string bounds
    rect.size = textBounds;

    ignoreInput = true;
}

Label::Label(const std::string& text, int textSize, const Rect& rect) : Widget{rect}, text{text}, textSize{textSize} {
    SplitTextLines(text, textLines);
    UpdateLinesAndBounds();

    ignoreInput = true;
}

Label::Label(const std::string& text, int textSize, const glm::vec2& size) : Widget{size}, text{text}, textSize{textSize} {
    SplitTextLines(text, textLines);
    UpdateLinesAndBounds();

    ignoreInput = true;
}

void Label::Draw() {
    if (text.empty())
        return;

    Atlas* atlas {TextRenderer::GetAtlas(font)};
    if (!atlas) 
        return;

    if (clipText) {
        glEnable(GL_SCISSOR_TEST);
        Rect scaledRect{Camera::GetMainCamera().RectFromVirtual2ScreenSize(rect, true)};
        glScissor(static_cast<int>(scaledRect.position.x),
                  static_cast<int>(scaledRect.position.y),
                  static_cast<int>(scaledRect.size.x),
                  static_cast<int>(scaledRect.size.y));
    }

    // TextRenderer::RenderText(text, textSize, rect.position, appearance, settings, font);
    TextRenderer::RenderText(textLines, textSize, rect, textBounds, appearance, settings, horizontalAlign, verticalAlign, transform, font, atlas);

    if (clipText) {
        glDisable(GL_SCISSOR_TEST);
    }
}

void Label::SetText(const std::string& text) {
    this->text = text;
    SplitTextLines(text, textLines);
    UpdateLinesAndBounds();
}

void Label::SetFont(const Font& font) {
    this->font = font;
    UpdateLinesAndBounds();
}

void Label::SetTextSize(int size) {
    textSize = size;
    UpdateLinesAndBounds();
}

void Label::SetLetterSpacing(float value) {
    settings.letterSpacing = value;
    UpdateLinesAndBounds();
}

void Label::SetLineSpacing(float value) {
    settings.lineSpacing = value;
    UpdateLinesAndBounds();
}

void Label::SetWordSpacing(float value) {
    settings.wordSpacing = value;
    UpdateLinesAndBounds();
}

void Label::SetSpacesPerTab(int value) {
    settings.spacesPerTab = value;
    UpdateLinesAndBounds();
}


void Label::UpdateLinesAndBounds() {
    Atlas* atlas {TextRenderer::GetAtlas(font)};
    if (atlas) {
        textBounds = TextRenderer::GetTextBounds(textLines, textSize, settings, *atlas);
        LOG_TRACE("Bounds: {}, {}.", textBounds.x, textBounds.y);
    }
}