#ifndef __TEXT_H__
#define __TEXT_H__

#include "Common.hpp"

#include "Utils/Color.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct TextGradient {
    Color topLeftColor;
    Color topRightColor;
    Color bottomLeftColor;
    Color bottomRightColor;
};

enum class TextStyle {
    // Bold,    // This can be manually done
    // Italics, // This can depend on the font
    // Underline, // TODO
    // 
    Lowecase,
    Uppercase,
    SmallCaps
};

struct TextInfo {
    Color color; 
    
    bool useColorGradient   {false};                                
    TextGradient colorGradient;
    float width             {0.45f};  
    float edge              {0.1f};
    float borderWidth       {0.6f}; // Setting to 0 make the border disappear
    float borderEdge        {0.1f}; //! Don't make this 0
    glm::vec2 borderOffset  {0.0f}; // Useful for drop shadows
    Color outlineColor;

    float letterSpacing {0};
    float lineSpacing   {0};
    float wordSpacing   {0};
    int spacesPerTab    {4};

    // bool wrap            {false};
};

enum class TextHorzAlign {
    Left, Center, Right
};

enum class TextVerticalAlign {
    Top, Center, Bottom
};

class Text {

};

#endif // __TEXT_H__