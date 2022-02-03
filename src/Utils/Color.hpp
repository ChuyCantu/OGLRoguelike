#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdint.h>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Color {
    union {
        uint32_t c;
        struct {
            uint8_t a;
            uint8_t b;
            uint8_t g;
            uint8_t r;
        };
    };

    Color();
    // Color always consider the 4 channels RGBA in that order
    Color(uint32_t color);
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    Color(float r, float g, float b, float a = 1.f);
};

// Utility function
Color Hex2RGB(const std::string& hex);
Color Hex2RGBA(const std::string& hex);

glm::vec3 Color2Vec3(const Color& color);
glm::vec4 Color2Vec4(const Color& color);

void DebugColorRGB(const std::string& label, Color& color, bool compact = true);
void DebugColorRGBA(const std::string& label, Color& color, bool compact = true);

#endif // __COLOR_H__