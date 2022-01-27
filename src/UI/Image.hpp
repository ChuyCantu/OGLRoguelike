#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "Common.hpp"
#include "Widget.hpp"

class Sprite;

class Image : public Widget {
public:
    Image();
    Image(const Rect& rect, Ref<Sprite> sprite);

    void Draw() override;

protected:
    Ref<Sprite> sprite;
};

#endif // __IMAGE_H__