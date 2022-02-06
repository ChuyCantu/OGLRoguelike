#ifndef __TEXTRENDERER_H__
#define __TEXTRENDERER_H__

#include "Common.hpp"
#include "Utils/Color.hpp"
#include "UI/Rect.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct TextGradient {
    Color topLeftColor;
    Color topRightColor;
    Color bottomLeftColor;
    Color bottomRightColor;
};

enum class TextTransform {
    // Bold,    // This can be manually done
    // Italics, // This can depend on the font
    // Underline, // TODO
    // Stroke, // TODO
    //
    Lowecase,
    Uppercase,
    SmallCaps,
    InvertCaps,
    None
};

struct TextAppearance {
    Color color;
    bool useColorGradient{false};
    TextGradient colorGradient;

    float width{0.45f};
    float edge{0.1f};
    float borderWidth{0.6f};       // Setting to 0 make the border disappear
    float borderEdge{0.1f};        //! Don't make this 0
    glm::vec2 borderOffset{0.0f};  // Useful for drop shadows
    Color outlineColor;
};

struct TextSettings {
    float letterSpacing{0};
    float lineSpacing{0};
    float wordSpacing{0};
    int spacesPerTab{4};

    // bool wrap            {false};
};

enum class TextHorzAlign {
    Left,
    Center,
    Right
};

enum class TextVertAlign {
    Top,
    Center,
    Bottom
};

struct LineInfo {
    std::string text;
    glm::vec2 size;
};

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
    static void RenderText(const std::string& text, float size, const glm::vec2& position, const TextAppearance& textAppearance, const TextSettings& settings, const Font& font);

    static glm::vec2 GetTextBounds(const std::vector<std::string>& text, float size, const TextSettings& settings, Atlas& atlas);
    static glm::vec2 GetLineBounds(const std::string& text, float size, const TextSettings& settings, Atlas& atlas);

    // Text position represents the top-left point of the bounding rectangle position, so it's easier to work with UI widgets
    static void RenderText(std::vector<LineInfo>& text, float size, const Rect& rect, const glm::vec2 textBounds,
                           const TextAppearance& textAppearance, const TextSettings& settings, TextHorzAlign horzAlign, TextVertAlign vertAlign, TextTransform transform,
                           const Font& font, const Atlas* atlas);
    static glm::vec2 GetTextBounds(std::vector<LineInfo>& text, float size, const TextSettings& settings, Atlas& atlas);
    static void CalculateLineBounds(LineInfo& line, float size, const TextSettings& settings, Atlas& atlas);

    static Atlas* GetAtlas(const Font& font);

private: 
    static FontMap fonts;    
};

void SplitTextLines(const std::string& text, std::vector<LineInfo>& outLines);
void SplitTextLines(const std::string& text, std::vector<std::string>& outLines);

void DebugTextInfoWindow(const std::string& label, TextAppearance& textAppearance, TextSettings& settings);

#endif // __TEXTRENDERER_H__
