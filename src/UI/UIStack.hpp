#ifndef __UISTACK_H__
#define __UISTACK_H__

#include "Common.hpp"
#include "Panel.hpp"

#include <vector>

// TODO: Add support for finding panels and widgets
// TODO: Make each panel render to its own texture and only render that texture if no UI element has changed
class UIStack {
public: 
    Panel* AddPanel(Owned<Panel> panel);
    void RemovePanel(Panel* panel);
    void Update();
    void RenderPanels();

    void OnRenderOrderChanged();

private:
    std::vector<Owned<Panel>> panels;
    bool needReordering {true};
};

#endif // __UISTACK_H__