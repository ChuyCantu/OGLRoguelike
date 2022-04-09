#include "Color.hpp"

#include "Core/Log.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

Color::Color() : a{255}, b{0}, g{0}, r{0} { }

Color::Color(uint32_t color) : c{color} { }

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) 
    : a{a}, b{b}, g{g}, r{r} { }

Color::Color(float r, float g, float b, float a)
    : a{static_cast<uint8_t>(std::clamp(a, 0.0f, 1.0f) * 255)},
      b{static_cast<uint8_t>(std::clamp(b, 0.0f, 1.0f) * 255)},
      g{static_cast<uint8_t>(std::clamp(g, 0.0f, 1.0f) * 255)},
      r{static_cast<uint8_t>(std::clamp(r, 0.0f, 1.0f) * 255)} { }

Color Hex2RGB(const std::string& hex) {
    std::string s {hex[0] == '#' ? /*"0x" + */hex.substr(1, hex.size() - 1) : hex};
    unsigned int value; 
    try {
        value = static_cast<unsigned int>(std::stoul(s, nullptr, 16));
    } catch (std::exception& e) {
        LOG_CRITICAL("Failed to convert string '{}' to a rgb color:  {}.", hex, e.what());
        value = 0;
    }
    
    Color color;
    color.b = value;
    color.g = value >> 8;
    color.r = value >> 16;

    return color;
}

Color Hex2RGBA(const std::string& hex) {
    std::string s {hex[0] == '#' ? /*"0x" + */hex.substr(1, hex.size() - 1) : hex};
    unsigned int value;
    try {
        value = static_cast<unsigned int>(std::stoul(s, nullptr, 16));
    } catch (std::exception& e) {
        LOG_CRITICAL("Failed to convert string '{}' to a rgba color:  {}.", hex, e.what());
        value = 0;
    }
    
    Color color;
    color.a = value;
    color.b = value >> 8;
    color.g = value >> 16;
    color.r = value >> 24;

    return color;
}

glm::vec3 Color2Vec3(const Color& color) {
    return glm::vec3{color.r / 255.0f, color.g / 255.0f, color.b / 255.0f};

    //+ With bit-shifting:
    // return glm::vec3{
    //     (color.c >> 24)             / 255.0f,
    //     ((color.c >> 16) & 0x00ff)  / 255.0f,
    //     ((color.c >> 8) & 0x0000ff) / 255.0f,
    // };
}

glm::vec4 Color2Vec4(const Color& color) {
    return glm::vec4{color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};

    //+ With bit-shifting:
    // return glm::vec4{
    //     (color.c >> 24)             / 255.0f,
    //     ((color.c >> 16) & 0x00ff)  / 255.0f,
    //     ((color.c >> 8) & 0x0000ff) / 255.0f,
    //     (color.c & 0x000000ff)      / 255.0f
    // };
}

Color Vec3ToColor(const glm::vec3& color) {
    return Color{static_cast<uint8_t>(std::clamp(color.r, 0.0f, 1.0f) * 255),
                 static_cast<uint8_t>(std::clamp(color.g, 0.0f, 1.0f) * 255),
                 static_cast<uint8_t>(std::clamp(color.b, 0.0f, 1.0f) * 255)};
}

Color Vec4ToColor(const glm::vec4& color) {
    return Color{static_cast<uint8_t>(std::clamp(color.r, 0.0f, 1.0f) * 255),
                 static_cast<uint8_t>(std::clamp(color.g, 0.0f, 1.0f) * 255),
                 static_cast<uint8_t>(std::clamp(color.b, 0.0f, 1.0f) * 255),
                 static_cast<uint8_t>(std::clamp(color.a, 0.0f, 1.0f) * 255)};
}


void DebugColorRGB(const std::string& label, Color& color, bool compact) {
    glm::vec3 testColorv3{Color2Vec3(color)};

    if (compact)
        ImGui::ColorEdit3(label.c_str(), glm::value_ptr(testColorv3));
    else
        ImGui::ColorPicker3(label.c_str(), glm::value_ptr(testColorv3));

    color = Color{testColorv3.r, testColorv3.g, testColorv3.b};
}

void DebugColorRGBA(const std::string& label, Color& color, bool compact) {
    glm::vec4 testColorv4{Color2Vec4(color)};

    if (compact)
        ImGui::ColorEdit4(label.c_str(), glm::value_ptr(testColorv4));
    else
        ImGui::ColorPicker4(label.c_str(), glm::value_ptr(testColorv4));

    color = Color{testColorv4.r, testColorv4.g, testColorv4.b, testColorv4.a};
}
