#include "TextRenderer.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Log.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/Batch.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/gtc/matrix_transform.hpp>

FontMap TextRenderer::fonts;

void TextRenderer::LoadFont(const std::string& fontFile, const std::string& name, int fontSize, FontRenderMode renderMode) {
    if (fontSize == 0) {
        LOG_WARN("Could not create font with a size of 0.");
        return;
    }
    
    FT_Library ft;

    if (FT_Init_FreeType(&ft)) {
        LOG_WARN("Could not initialize FreeType library.");
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontFile.c_str(), 0, &face)) {
        LOG_WARN("Could not open font {}.", fontFile);
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int charactersNum {128}; // Hardcoded in here because for now this only works with ASCII
    int padding {2};
    
    Atlas atlas;
    atlas.size = glm::ivec2{padding, padding * 2}; // Initial size
    atlas.baseFontSize = fontSize;
    atlas.characters = std::vector<CharacterInfo>(charactersNum);
    atlas.maxBearing = glm::ivec2{0};

    std::vector<std::vector<uint8_t>> glyphsBuffer(charactersNum);

    // Caracters with code lower than 33 are control characters only
    for (uint8_t c {33}; c < charactersNum; ++c) {
        if (renderMode == FontRenderMode::SDF) {
            if (FT_Load_Char(face, c, FT_LOAD_DEFAULT)) {
                LOG_CRITICAL("Could not load character '{}'.", c);
                // return;
            }

            if (FT_Render_Glyph(face->glyph, FT_Render_Mode::FT_RENDER_MODE_SDF)) {
                LOG_CRITICAL("Could not load character '{}' as SDF.", c);
                // return;
            }
        }
        else if (renderMode == FontRenderMode::Raster) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                LOG_CRITICAL("Could not load character '{}'.", c);
                // return;
            }
        }

        atlas.size.x += face->glyph->bitmap.width + padding;
        atlas.size.y = std::max((uint32_t)atlas.size.y, face->glyph->bitmap.rows + padding * 2);

        // Temporarily cache glyph buffers
        uint32_t bufferSize {face->glyph->bitmap.width * face->glyph->bitmap.rows};
        glyphsBuffer[c] = std::vector<uint8_t>(bufferSize);
        memcpy(glyphsBuffer[c].data(), face->glyph->bitmap.buffer, bufferSize);

        // Fill initial character info
        atlas.characters[c].size = glm::ivec2{face->glyph->bitmap.width, face->glyph->bitmap.rows};
        atlas.characters[c].bearing = glm::ivec2{face->glyph->bitmap_left, face->glyph->bitmap_top};
        atlas.characters[c].advance = glm::ivec2{face->glyph->advance.x, face->glyph->advance.y};

        atlas.maxBearing.x = std::max(atlas.maxBearing.x, face->glyph->bitmap_left);
        atlas.maxBearing.y = std::max(atlas.maxBearing.y, face->glyph->bitmap_top);
    }

    atlas.metricsWidth = face->size->metrics.max_advance;
    atlas.metricsHeight = face->size->metrics.height;

    atlas.texture = MakeRef<Texture>();
    atlas.texture->Generate(atlas.size.x, atlas.size.y, nullptr, TextureFormat::R8, TextureFormat::RED);
    atlas.texture->SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge).
                SetMinFilter(TextureParameter::Linear).SetMagFilter(TextureParameter::Linear);

    // Generate texture atlas and set characters uv coords (for now is a single row, so y is always = padding)
    uint32_t x {static_cast<uint32_t>(padding)};
    for (uint8_t c{32}; c < 128; ++c) {
        atlas.characters[c].uv = glm::vec2{(float)x / atlas.size.x, padding};

        // Upload glyph buffer to atlas texture
        atlas.texture->SubImage(x, padding, atlas.characters[c].size.x, atlas.characters[c].size.y, glyphsBuffer[c].data());

        x += atlas.characters[c].size.x + padding;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    switch (renderMode) {
        case FontRenderMode::SDF:
            fonts[name][0] = atlas; // Size 0 index is reserved for SDF 
            break;
        case FontRenderMode::Raster:
            fonts[name][fontSize] = atlas;
            break;
    }
}

void TextRenderer::RenderText(const std::string& text, float size, const glm::vec2& position, const TextAppearance& textAppearance, const TextSettings& settings, const Font& font) {
    if (text.empty())
        return;

    auto shader {AssetManager::GetShader("text")};
    if (font.mode == FontRenderMode::SDF) {
        shader = AssetManager::GetShader("textSDF");
    }
    shader->Use();

    Atlas* atlas {nullptr};

    if (font.mode == FontRenderMode::Raster) {
        auto fontIter {fonts.find(font.name)};
        if (fontIter != fonts.end()) {
            auto atlasIter {fontIter->second.find(font.size)};
            if (atlasIter != fontIter->second.end()) {
                atlas = &atlasIter->second;
            }
            else {
                LOG_WARN("No font with name '{}' and size '{}' was loaded.", font.name, font.size);
                return;
            }
        }
    }
    else {
        auto fontIter {fonts.find(font.name)};
        if (fontIter != fonts.end()) {
            auto atlasIter {fontIter->second.find(0)};
            if (atlasIter != fontIter->second.end()) {
                atlas = &atlasIter->second;
            }
            else {
                LOG_WARN("No sdf font with name '{}' was loaded.", font.name);
                return;
            }
        }

        shader->SetFloat("textInfo.width", textAppearance.width);
        shader->SetFloat("textInfo.edge", textAppearance.edge);
        shader->SetFloat("textInfo.borderWidth", textAppearance.borderWidth);
        shader->SetFloat("textInfo.borderEdge", textAppearance.borderEdge);
        shader->SetVec2("textInfo.borderOffset", textAppearance.borderOffset);
        shader->SetVec3("textInfo.outlineColor", Color2Vec3(textAppearance.outlineColor));
    }

    atlas->texture->Use();

    glm::vec2 scale {size / (float)atlas->baseFontSize};

    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float penx {position.x};
    float peny {position.y + atlas->maxBearing.y * scale.y};
    TextBatch::Start();
    for (auto c {text.begin()}; c != text.end(); ++c) {
        CharacterInfo& ch {atlas->characters[static_cast<uint8_t>(*c)]};

        switch (*c) {
            case ' ':
                penx += settings.wordSpacing + (atlas->metricsWidth / 2 >> 6) * scale.x;
                continue;
            case '\n':
                penx = position.x;
                peny += settings.lineSpacing + (atlas->metricsHeight >> 6) * scale.y;
                continue;
            case '\t':
                penx += (settings.wordSpacing + (atlas->metricsWidth / 2 >> 6) * scale.x) * settings.spacesPerTab;
                continue;
        }

        // Positions relative to the origin line for the glyph:
        float xpos {penx + ch.bearing.x * scale.x};
        float ypos {peny - ch.bearing.y * scale.y};

        float w    {ch.size.x * scale.x};
        float h    {ch.size.y * scale.y};

        TextVertex tl{
            glm::vec4{xpos, ypos,
                      ch.uv.x, ch.uv.y / (float)atlas->size.y},
        };
        TextVertex tr{
            glm::vec4{xpos + w, ypos,
                      ch.uv.x + ch.size.x / (float)atlas->size.x, ch.uv.y / (float)atlas->size.y},
        };
        TextVertex bl {
            glm::vec4{xpos, ypos + h, 
                      ch.uv.x, (ch.uv.y + ch.size.y) / (float)atlas->size.y},
        };
        TextVertex br {
            glm::vec4{xpos + w, ypos + h, 
                      ch.uv.x + ch.size.x / (float)atlas->size.x, (ch.uv.y + ch.size.y) / (float)atlas->size.y},
        };

        if (textAppearance.useColorGradient) {
            tr.color = Color2Vec4(textAppearance.colorGradient.topRightColor);
            tl.color = Color2Vec4(textAppearance.colorGradient.topLeftColor);
            bl.color = Color2Vec4(textAppearance.colorGradient.bottomLeftColor); 
            br.color = Color2Vec4(textAppearance.colorGradient.bottomRightColor); 
        }
        else {
            bl.color = Color2Vec4(textAppearance.color);
            br.color = Color2Vec4(textAppearance.color);
            tl.color = Color2Vec4(textAppearance.color);
            tr.color = Color2Vec4(textAppearance.color);
        }

        TextBatch::AddCharacter(bl, br, tl, tr);

        penx += settings.letterSpacing + (ch.advance.x >> 6) * scale.x;  // Bitshift by 6 to get a value in pixels (1/64th times 2^6 = 64)
        peny += (ch.advance.y >> 6) * scale.y; 
    }
    TextBatch::Flush();
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void TextRenderer::RenderText(std::vector<LineInfo>& text, float size, const Rect& rect, const glm::vec2 textBounds,
                              const TextAppearance& textAppearance, const TextSettings& settings, TextHorzAlign horzAlign, TextVertAlign vertAlign, TextTransform transform,
                              const Font& font, const Atlas* atlas) {
    if (text.empty())
        return;

    auto shader {AssetManager::GetShader("text")};
    if (font.mode == FontRenderMode::SDF) {
        shader = AssetManager::GetShader("textSDF");
    }
    shader->Use();

    if (font.mode == FontRenderMode::SDF) {
        shader->SetFloat("textInfo.width", textAppearance.width);
        shader->SetFloat("textInfo.edge", textAppearance.edge);
        shader->SetFloat("textInfo.borderWidth", textAppearance.borderWidth);
        shader->SetFloat("textInfo.borderEdge", textAppearance.borderEdge);
        shader->SetVec2("textInfo.borderOffset", textAppearance.borderOffset);
        shader->SetVec3("textInfo.outlineColor", Color2Vec3(textAppearance.outlineColor));
    }

    atlas->texture->Use();

    glm::vec2 scale {size / (float)atlas->baseFontSize};

    // glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glm::vec2 pen {0.0f};
    // pen.y = rect.position.y + atlas->maxBearing.y * scale.y;

    switch (vertAlign) {
        case TextVertAlign::Top:
            pen.y = rect.position.y + atlas->maxBearing.y * scale.y;
            break;
        case TextVertAlign::Center:
            pen.y = rect.position.y + atlas->maxBearing.y * scale.y + (rect.size.y - textBounds.y) / 2.0f;
            break;
        case TextVertAlign::Bottom:
            pen.y = rect.position.y + atlas->maxBearing.y * scale.y + (rect.size.y - textBounds.y);
            break;
    }

    TextBatch::Start();
    for (size_t i{}; i < text.size(); ++i) {
        const LineInfo& currentLine{text[i]};

        switch (horzAlign) {
            case TextHorzAlign::Left:
                pen.x = rect.position.x;
                break;
            case TextHorzAlign::Center:
                pen.x = rect.position.x + (rect.size.x - currentLine.size.x) / 2.0f;
                break;
            case TextHorzAlign::Right:
                pen.x = rect.position.x + (rect.size.x - currentLine.size.x);
                break;
        }

        // for (auto c {currentLine.text.begin()}; c != currentLine.text.end(); ++c) {
        for (size_t i {0}; i < currentLine.text.size(); ++i) {
            char c {currentLine.text[i]};

            switch (transform) {
                case TextTransform::None:
                    break;
                case TextTransform::Lowecase:
                    if (c >= 'A' && c <= 'Z') {
                        c -= ('A' - 'a');
                    }
                    break;
                case TextTransform::Uppercase:
                    if (c >= 'a' && c <= 'z') {
                        c += ('A' - 'a');
                    }
                    break;
                case TextTransform::InvertCaps:
                    if (c >= 'A' && c <= 'Z') {
                        c -= ('A' - 'a');
                    }
                    else if (c >= 'a' && c <= 'z') {
                        c += ('A' - 'a');
                    }
                    break;
                case TextTransform::SmallCaps: // TODO: Implement small caps text transformation
                    break;
            }

            const CharacterInfo& ch {atlas->characters[c]};

            switch (c) {
                case ' ':
                    pen.x += settings.wordSpacing + (atlas->metricsWidth / 2 >> 6) * scale.x;
                    continue;
                case '\t':
                    pen.x += (settings.wordSpacing + (atlas->metricsWidth / 2 >> 6) * scale.x) * settings.spacesPerTab;
                    continue;
            }

            // Positions relative to the origin line for the glyph:
            float xpos{pen.x + ch.bearing.x * scale.x};
            float ypos{pen.y - ch.bearing.y * scale.y};

            float w{ch.size.x * scale.x};
            float h{ch.size.y * scale.y};

            TextVertex tl{
                glm::vec4{xpos, ypos,
                          ch.uv.x, ch.uv.y / (float)atlas->size.y},
            };
            TextVertex tr{
                glm::vec4{xpos + w, ypos,
                          ch.uv.x + ch.size.x / (float)atlas->size.x, ch.uv.y / (float)atlas->size.y},
            };
            TextVertex bl{
                glm::vec4{xpos, ypos + h,
                          ch.uv.x, (ch.uv.y + ch.size.y) / (float)atlas->size.y},
            };
            TextVertex br{
                glm::vec4{xpos + w, ypos + h,
                          ch.uv.x + ch.size.x / (float)atlas->size.x, (ch.uv.y + ch.size.y) / (float)atlas->size.y},
            };

            if (textAppearance.useColorGradient) {
                tr.color = Color2Vec4(textAppearance.colorGradient.topRightColor);
                tl.color = Color2Vec4(textAppearance.colorGradient.topLeftColor);
                bl.color = Color2Vec4(textAppearance.colorGradient.bottomLeftColor);
                br.color = Color2Vec4(textAppearance.colorGradient.bottomRightColor);
            } 
            else {
                bl.color = Color2Vec4(textAppearance.color);
                br.color = Color2Vec4(textAppearance.color);
                tl.color = Color2Vec4(textAppearance.color);
                tr.color = Color2Vec4(textAppearance.color);
            }

            TextBatch::AddCharacter(bl, br, tl, tr);

            pen.x += settings.letterSpacing + (ch.advance.x >> 6) * scale.x;  // Bitshift by 6 to get a value in pixels (1/64th times 2^6 = 64)
            pen.y += (ch.advance.y >> 6) * scale.y;
        }
        pen.x = rect.position.x;
        pen.y += settings.lineSpacing + (atlas->metricsHeight >> 6) * scale.y;
    }
    TextBatch::Flush();
    glDisable(GL_CULL_FACE);
    // glDisable(GL_BLEND);
}

Atlas* TextRenderer::GetAtlas(const Font& font) {
    Atlas* atlas {nullptr};

    if (font.mode == FontRenderMode::Raster) {
        auto fontIter {fonts.find(font.name)};
        if (fontIter != fonts.end()) {
            auto atlasIter {fontIter->second.find(font.size)};
            if (atlasIter != fontIter->second.end()) {
                atlas = &atlasIter->second;
            }
            else {
                LOG_WARN("No font with name '{}' and size '{}' was loaded.", font.name, font.size);
            }
        }
    }
    else {
        auto fontIter {fonts.find(font.name)};
        if (fontIter != fonts.end()) {
            auto atlasIter {fontIter->second.find(0)};
            if (atlasIter != fontIter->second.end()) {
                atlas = &atlasIter->second;
            }
            else {
                LOG_WARN("No sdf font with name '{}' was loaded.", font.name);
            }
        }
    }

    return atlas;
}

// TODO:
glm::vec2 TextRenderer::GetTextBounds(const std::vector<std::string>& text, float size, const TextSettings& settings, Atlas& atlas) {
    glm::vec2 scale {size / (float)atlas.baseFontSize};

    glm::vec2 bbox {0.0f};

    for (auto& str : text) {
        glm::vec2 lineBounds {GetLineBounds(str, size, settings, atlas)};
        bbox.x = std::max(bbox.x, lineBounds.x);
        bbox.y += lineBounds.y;
    }

    return bbox;
}

glm::vec2 TextRenderer::GetTextBounds(std::vector<LineInfo>& text, float size, const TextSettings& settings, Atlas& atlas) {
    glm::vec2 bbox {0.0f};

    for (auto& line : text) {
        CalculateLineBounds(line, size, settings, atlas);
        bbox.x = std::max(bbox.x, line.size.x);
        bbox.y += line.size.y;
    }

    return bbox;
}

//! Consider blank strings ("") as 0 width but with a height
glm::vec2 TextRenderer::GetLineBounds(const std::string& text, float size, const TextSettings& settings, Atlas& atlas) {
    glm::vec2 scale {size / (float)atlas.baseFontSize};   

    glm::vec2 bbox {0.0f, (atlas.metricsHeight >> 6) * scale.y};

    if (text.empty())
        return bbox;

    for (auto i {0}; i < text.size(); ++i) {
        char c {text[i]};
        CharacterInfo& ch {atlas.characters[c]};

        switch (c) {
            case ' ':
                bbox.x += settings.wordSpacing + (atlas.metricsWidth / 2 >> 6) * scale.x;
                continue;
            case '\t':
                bbox.x += (settings.wordSpacing + (atlas.metricsWidth / 2 >> 6) * scale.x) * settings.spacesPerTab;
                continue;
            case '\n':
                return bbox;
        }

        bbox.x += settings.letterSpacing + (ch.advance.x >> 6) * scale.x;  // Bitshift by 6 to get a value in pixels (1/64th times 2^6 = 64)
    }

    return bbox;
}

// TODO: Consider text transformation
void TextRenderer::CalculateLineBounds(LineInfo& line, float size, const TextSettings& settings, Atlas& atlas) {
    glm::vec2 scale {size / (float)atlas.baseFontSize};   

    line.size = glm::vec2{0.0f, (atlas.metricsHeight >> 6) * scale.y};

    if (line.text.empty())
        return;

    for (auto i{0}; i < line.text.size(); ++i) {
        char c {line.text[i]};
        CharacterInfo& ch {atlas.characters[c]};

        switch (c) {
            case ' ':
                line.size.x += settings.wordSpacing + (atlas.metricsWidth / 2 >> 6) * scale.x;
                continue;
            case '\t':
                line.size.x += (settings.wordSpacing + (atlas.metricsWidth / 2 >> 6) * scale.x) * settings.spacesPerTab;
                continue;
            case '\n':
                return;
        }

        line.size.x += settings.letterSpacing + (ch.advance.x >> 6) * scale.x;  // Bitshift by 6 to get a value in pixels (1/64th times 2^6 = 64)
    }
    line.size.x = std::ceil(line.size.x);
    line.size.y = std::ceil(line.size.y);
}

void SplitTextLines(const std::string& text, std::vector<LineInfo>& outLines) {
    outLines.clear();

    if (text.empty())
        return;

    size_t lineStart {0};
    size_t lineEnd   {0};

    while ((lineEnd = text.find('\n', lineStart)) != std::string::npos) {
        outLines.emplace_back(LineInfo{text.substr(lineStart, lineEnd - lineStart)});
        lineStart = lineEnd + 1;
    }

    if (lineStart < text.size())
        outLines.emplace_back(LineInfo{text.substr(lineStart, text.size() - lineStart)});

    if (text[text.size() - 1] == '\n')
        outLines.emplace_back(LineInfo{""});
}

void SplitTextLines(const std::string& text, std::vector<std::string>& outLines) {
    outLines.clear();

    if (text.empty())
        return;

    size_t lineStart {0};
    size_t lineEnd   {0};

    while ((lineEnd = text.find('\n', lineStart)) != std::string::npos) {
        outLines.emplace_back(text.substr(lineStart, lineEnd - lineStart));
        lineStart = lineEnd + 1;
    }

    if (lineStart < text.size())
        outLines.emplace_back(text.substr(lineStart, text.size() - lineStart));

    if (text[text.size() - 1] == '\n')
        outLines.emplace_back("");
}

void DebugTextInfoWindow(const std::string& label, TextAppearance& textAppearance, TextSettings& settings) {
    ImGui::Begin(label.c_str());
    DebugColorRGB("color", textAppearance.color);
    ImGui::Checkbox("color gradient?", &textAppearance.useColorGradient);
    DebugColorRGB("color tl", textAppearance.colorGradient.topLeftColor);
    DebugColorRGB("color tr", textAppearance.colorGradient.topRightColor);
    DebugColorRGB("color bl", textAppearance.colorGradient.bottomLeftColor);
    DebugColorRGB("color br", textAppearance.colorGradient.bottomRightColor);
    ImGui::InputFloat("width", &textAppearance.width);
    ImGui::InputFloat("edge", &textAppearance.edge);
    ImGui::InputFloat("borderWidth", &textAppearance.borderWidth);
    ImGui::InputFloat("borderEdge", &textAppearance.borderEdge);
    ImGui::InputFloat2("borderOffset", glm::value_ptr(textAppearance.borderOffset));
    DebugColorRGB("borderColor", textAppearance.outlineColor);
    ImGui::InputFloat("letterSpacing", &settings.letterSpacing);
    ImGui::InputFloat("wordSpacing", &settings.wordSpacing);
    ImGui::InputFloat("lineSpacing", &settings.lineSpacing);
    ImGui::InputInt("spacesPerTab", &settings.spacesPerTab);
    ImGui::End();
}