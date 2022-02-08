#ifndef __CHECKBOX_H__
#define __CHECKBOX_H__

#include "Image.hpp"
#include "Widget.hpp"

class Checkbox : public Widget {
public:
    Checkbox();
    Checkbox(const Rect& rect);
    Checkbox(const glm::vec2& size);
    ~Checkbox() override;

    void Draw() override;

protected:
    void SetupDefaultValues();
    void UpdateCheckboxChildrenSize(Widget* source);

    void ToggleSChecked(Widget* source, EventHandler& eventHandler);

public:
    bool checked {false};
    Event<void(Widget*, bool)> onValueChanged;

private:
    Image* checkedImg;
    Image* uncheckedImg;
};

#endif // __CHECKBOX_H__