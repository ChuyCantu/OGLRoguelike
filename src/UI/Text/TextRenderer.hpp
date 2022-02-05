#ifndef __TEXTRENDERER_H__
#define __TEXTRENDERER_H__

#include "Common.hpp"

#include "Text.hpp"
#include "Utils/Color.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

enum class FontRenderMode {
    SDF,
    Raster
};

struct Font {
    std::string name;
    int size;
    FontRenderMode mode;
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

    int metricsWidth;
    int metricsHeight;
    glm::ivec2 maxBearing;
};

using FontAtlasMap = std::unordered_map<int, Atlas>;
using FontMap = std::unordered_map<std::string, FontAtlasMap>;

// TODO: Support caching the atlas texture on the computer
class TextRenderer {
public:
    // When renderMode is SDF fontSize is recommended to be left as 64 since bigger sizes take longer to load and 64 is already produces really good results
    static void LoadFont(const std::string& fontFile, const std::string& name, int fontSize = 64, FontRenderMode renderMode = FontRenderMode::SDF);
    
    // Text position represents the top-left point of the bounding rectangle position, so it's easier to work with UI widgets
    static void RenderText(const std::string& text, float size, const glm::vec2& position, const TextInfo& textInfo, const Font& font);

    static glm::vec2 GetTextBounds(const std::string& text, float size, const TextInfo& textInfo, Atlas& atlas);
    static glm::vec2 GetLineBounds(const std::string& text, float size, const TextInfo& textInfo, Atlas& atlas, size_t start, size_t& outLineEnd);

   private: 
    static FontMap fonts;
};

void DebugTextInfoWindow(const std::string& label, TextInfo& textInfo);

#endif // __TEXTRENDERER_H__
