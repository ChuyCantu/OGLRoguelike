#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "Common.hpp"
#include "Widget.hpp"

#include <glm/vec4.hpp>

class Sprite;

class Image : public Widget {
public:
    Image();
    Image(const Rect& rect, Ref<Sprite> sprite);
    Image(const Rect& rect, const std::vector<Ref<class Sprite>>& sprites);
    Image(const Rect& rect, const std::vector<Ref<class Sprite>>&& sprites);

    void Draw() override;

    // If useNineSlice is set to false, this sprite is used, otherwise, it is ignored and should not be accessed
    Ref<Sprite> sprite;

    bool useNineSlice {false};
    
    // If useNineSlice is set to false, this is ignored, otherwise, the list should follow the next order:
    // topLeft, topCenter, topRight,
    // centerLeft, center, centerRight, 
    // bottomLeft, bottomCenter, bottomRight
    std::vector<Ref<class Sprite>> slicedSprites;
    glm::vec4 color {1.0f};
};

#endif // __IMAGE_H__