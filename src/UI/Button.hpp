#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "Image.hpp"
#include "Label.hpp"
#include "Widget.hpp"

class Button : public Widget {
public:
    Button(const std::string& text);
    Button(const std::string& text, const Rect& rect);
    Button(const std::string& text, const glm::vec2& size);   
    ~Button() override; 

    void Draw() override;

    const Image* NormalImage() { return normal; }
    const Image* HighlightedImage() { return highlighted; }
    const Image* DisabledImage() { return disabled; }

protected:
    void SetupDefaultValues();
    void UpdateButtonChildrenSize(Widget* source);

private:
    Label* label;
    Image* normal;
    Image* pressed;
    Image* highlighted;
    Image* disabled;
};

#endif // __BUTTON_H__