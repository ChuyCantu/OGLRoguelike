#include "Label.hpp"

#include "Core/Log.hpp"
#include "Rendering/Camera.hpp"
#include "Text/TextRenderer.hpp"

#include <glad/glad.h>

Label::Label(const std::string& text) : Widget{Rect{glm::vec2{0.0f}, glm::vec2{0.0f}}}, text{text} { 
    // Actual size is computed by the string bounds

    SplitTextLines(text, textLines);

    // Calculate:
    Atlas* atlas {TextRenderer::GetAtlas(font)};
    if (atlas) {
        glm::vec2 bounds {TextRenderer::GetTextBounds(textLines, font.size, settings, *atlas)};
        rect.size = bounds;
        LOG_TRACE("Bounds: {}, {}.", bounds.x, bounds.y);
    }
}

Label::Label(const std::string& text, const Rect& rect) : Widget{rect}, text{text} {
    SplitTextLines(text, textLines);
}

Label::Label(const std::string& text, const glm::vec2& size) : Widget{size}, text{text} {
    SplitTextLines(text, textLines);
}

void Label::Draw() {
    if (clipText) {
        glEnable(GL_SCISSOR_TEST);
        Rect scaledRect{Camera::GetMainCamera().RectFromVirtual2ScreenSize(rect, true)};
        glScissor(static_cast<int>(scaledRect.position.x),
                  static_cast<int>(scaledRect.position.y),
                  static_cast<int>(scaledRect.size.x),
                  static_cast<int>(scaledRect.size.y));
    }

    TextRenderer::RenderText(text, 22, rect.position, appearance, settings, font);

    if (clipText) {
        glDisable(GL_SCISSOR_TEST);
    }
}