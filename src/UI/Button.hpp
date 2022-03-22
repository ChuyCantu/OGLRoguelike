#pragma once

#include "Image.hpp"
#include "Label.hpp"
#include "Widget.hpp"

class Button : public Widget {
public:
    Button(const std::string& text, const std::string& name = "Button");
    Button(const std::string& text, const Rect& rect, const std::string& name = "Button");
    Button(const std::string& text, const glm::vec2& size, const std::string& name = "Button");   
    ~Button() override; 

    void Draw() override;

    // const Label* GetLabel() { return label; }
    // const Image* NormalImage() { return normal; }
    // const Image* HighlightedImage() { return highlighted; }
    // const Image* DisabledImage() { return disabled; }

protected:
    void SetupDefaultValues();
    void UpdateButtonChildrenSize(Widget* source);

// private:
public:
    Label* label;
    Image* normal;
    Image* pressed;
    Image* highlighted;
    Image* disabled;
};