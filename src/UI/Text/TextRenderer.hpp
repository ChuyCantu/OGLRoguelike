#ifndef __TEXTRENDERER_H__
#define __TEXTRENDERER_H__

#include "Common.hpp"

#include "Utils/Color.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

enum class AtlasMode {
    SDF,
    Bitmap
};

struct Font {
    std::string name;
    int size;
    AtlasMode mode;
};

struct CharacterInfo {
    glm::ivec2 size;     // size of glyph
    glm::ivec2 bearing;  // offset from baseline to left/top of glyph
    glm::ivec2 advance;  // horizontal offset to advvance to next glyph
    glm::vec2 uv;        // texture coordinates on atlas texture of the top-left corner of the glyph
};

struct Atlas {
    Ref<class Texture> texture;
    glm::ivec2 size;
    int baseFontSize;
    std::vector<CharacterInfo> characters;
};

struct TextGradient {
    Color topLeftColor;
    Color topRightColor;
    Color bottomLeftColor;
    Color bottomRightColor;
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
};

using FontAtlasMap = std::unordered_map<int, Atlas>;
using FontMap = std::unordered_map<std::string, FontAtlasMap>;

class TextRenderer {
public:
    static void Init();

    static void LoadFont(const std::string& name, int fontSize, const std::string& fontFile);
    static void LoadFontSDF(const std::string& name, const std::string& fontFile, int fontBaseSize = 64);
    static void RenderText(const std::string& text, float size, const glm::vec2& position, const TextInfo& textInfo, const Font& font);

#ifdef ENABLE
    static void LoadFontAtlas(const std::string& fontFile, int fontSize);
    static void RenderTextAtlas(const std::string& text, const glm::vec2& position, const glm::vec2& scale = glm::vec2{1.f});

    static void LoadFontSDF(const std::string& fontFile, int fontSize);
    static void RenderTextSDF(const std::string& text, const glm::vec2& position, float size = 16.0f);
#endif

private: 
    static FontMap fonts;
};

void DebugTextInfoWindow(const std::string& label, TextInfo& textInfo);

#endif // __TEXTRENDERER_H__
