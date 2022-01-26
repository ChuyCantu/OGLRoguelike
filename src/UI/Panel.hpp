#ifndef __PANEL_H__
#define __PANEL_H__

#include "Common.hpp"
#include "Widget.hpp"

#include <vector>

class Panel {
public:
    Rect rect;

    std::vector<Owned<class Widget>> widgets;
};

#endif // __PANEL_H__