#ifndef __UI_H__
#define __UI_H__

#include "Common.hpp"

class UIStack;
class Widget;
class Panel;

class UI {
public:
    static void Init(UIStack* uiStack);

    static const UIStack* Stack() { return stack; };

    static Panel* AddPanel(Owned<Panel> panel);
    static void RemovePanel(Panel* panel);

    // static Panel* FindPanel(const std::string& name); // TODO

public:
    Widget* focused;
    Widget* hovered;

private:
    static UIStack* stack;
};

#endif // __UI_H__