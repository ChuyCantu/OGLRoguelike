#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "Common.hpp"
#include "Widget.hpp"
#include "Utils/Color.hpp"

#include <glm/vec4.hpp>

class Sprite;

class Image : public Widget {
public:
    Image(const std::string& name = "Image");
    Image(const Rect& rect, Ref<Sprite> sprite, const std::string& name = "Image");
    Image(const Rect& rect, const std::vector<Ref<class Sprite>>& sprites, const std::string& name = "Image");
    Image(const Rect& rect, const std::vector<Ref<class Sprite>>&& sprites, const std::string& name = "Image");

protected:
    void Draw() override;

public:
    // If useNineSlice is set to false, this sprite is used, otherwise, it is ignored and should not be accessed
    Ref<Sprite> sprite;

    bool useNineSlice {false};
    
    // If useNineSlice is set to false, this is ignored, otherwise, the list should follow the next order:
    // topLeft, topCenter, topRight,
    // centerLeft, center, centerRight, 
    // bottomLeft, bottomCenter, bottomRight
    std::vector<Ref<class Sprite>> slicedSprites;
    Color color {ColorNames::white};
};

#endif // __IMAGE_H__