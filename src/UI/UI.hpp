#ifndef __UI_H__
#define __UI_H__

#include "Common.hpp"

class UIStack;
class Widget;
class Panel;

class UI {
public:
    static void Init(UIStack* uiStack);

    static UIStack* const Stack() { return stack; };

    static Widget* AddPanel(Owned<Panel> panel);
    static void RemovePanel(Panel* panel);

    // static Panel* FindPanel(const std::string& name); // TODO

public:
    /**
     *  The static variables focused and hovered will take a pointer
     *  of the LAST widget that handled their corresponding events
     */
    static Widget* focused;
    static Widget* hovered;

private:
    static UIStack* stack;
};

#endif // __UI_H__