#ifndef __UISTACK_H__
#define __UISTACK_H__

#include "Common.hpp"
#include "Panel.hpp"

#include <vector>

class UIStack {
public: 
    std::vector<Owned<Panel>> panels;
};

#endif // __UISTACK_H__