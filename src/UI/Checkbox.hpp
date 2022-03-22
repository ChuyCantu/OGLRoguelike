#pragma once

#include "Image.hpp"
#include "Widget.hpp"

class Checkbox : public Widget {
public:
    Checkbox(const std::string& name = "Checkbox");
    Checkbox(const Rect& rect, const std::string& name = "Checkbox");
    Checkbox(const glm::vec2& size, const std::string& name = "Checkbox");
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