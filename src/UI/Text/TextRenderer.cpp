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

void TextRenderer::LoadFont(const std::string& name, int fontSize, const std::string& fontFile) {
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

    std::vector<std::vector<uint8_t>> glyphsBuffer(charactersNum);

    // Caracters with code lower than 32 are control characters only
    for (uint8_t c {32}; c < charactersNum; ++c) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            LOG_WARN("Could not load character '{}'.", c);
            return;
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
    }

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

    fonts[name][fontSize] = atlas;
}

void TextRenderer::LoadFontSDF(const std::string& name, const std::string& fontFile, int fontBaseSize) {
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

    FT_Set_Pixel_Sizes(face, 0, fontBaseSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int charactersNum {128}; // Hardcoded in here because for now this only works with ASCII
    int padding {2};
    
    Atlas atlas;
    atlas.size = glm::ivec2{padding, padding * 2}; // Initial size
    atlas.baseFontSize = fontBaseSize;
    atlas.characters = std::vector<CharacterInfo>(charactersNum);

    std::vector<std::vector<uint8_t>> glyphsBuffer(charactersNum);

    // Caracters with code lower than 32 are control characters only
    for (uint8_t c {33}; c < charactersNum; ++c) {
        if (FT_Load_Char(face, c, FT_LOAD_DEFAULT)) {
            LOG_CRITICAL("Could not load character '{}'.", c);
            return;
        }

        if (FT_Render_Glyph(face->glyph, FT_Render_Mode::FT_RENDER_MODE_SDF)) {
            LOG_CRITICAL("Could not load character '{}' as SDF.", c);
            return;
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
    }

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

    fonts[name][0] = atlas; // Size 0 is reserved for SDF 

    // Temp
    AssetManager::AddTexture("tempAtlas", atlas.texture);
}

void TextRenderer::RenderText(const std::string& text, float size, const glm::vec2& position, const TextInfo& textInfo, const Font& font) {
    auto shader {AssetManager::GetShader("text")};
    if (font.mode == AtlasMode::SDF) {
        shader = AssetManager::GetShader("textSDF");
    }
    shader->Use();

    Atlas* atlas;

    if (font.mode == AtlasMode::Bitmap) {
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

        shader->SetFloat("textInfo.width", textInfo.width);
        shader->SetFloat("textInfo.edge", textInfo.edge);
        shader->SetFloat("textInfo.borderWidth", textInfo.borderWidth);
        shader->SetFloat("textInfo.borderEdge", textInfo.borderEdge);
        shader->SetVec2("textInfo.borderOffset", textInfo.borderOffset);
        shader->SetVec3("textInfo.outlineColor", textInfo.outlineColor);
    }

    atlas->texture->Use();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float x {position.x};
    float y {position.y};
    TextBatch::Start();
    for (auto c {text.begin()}; c != text.end(); ++c) {
        CharacterInfo& ch {atlas->characters[static_cast<uint8_t>(*c)]};

        glm::vec2 scale {size / (float)atlas->baseFontSize};

        if (*c == '\n') {
            x = position.x;
            y -= atlas->characters['b'].size.y * scale.y;
            continue;
        } else if (*c == '\t') {
            x += atlas->characters['H'].size.x * 4;
            continue;
        }

        float xpos {x + ch.bearing.x * scale.x};
        float ypos {-y};
        float w    {ch.size.x * scale.x};
        float h    {ch.size.y * scale.y};

        TextVertex tl{
            glm::vec4{xpos, -ypos - h,
                      ch.uv.x, ch.uv.y / (float)atlas->size.y},
        };
        TextVertex tr{
            glm::vec4{xpos + w, -ypos - h,
                      ch.uv.x + ch.size.x / (float)atlas->size.x, ch.uv.y / (float)atlas->size.y},
        };
        TextVertex bl {
            glm::vec4{xpos, -ypos, 
                      ch.uv.x, (ch.uv.y + ch.size.y) / (float)atlas->size.y},
        };
        TextVertex br {
            glm::vec4{xpos + w, -ypos, 
                      ch.uv.x + ch.size.x / (float)atlas->size.x, (ch.uv.y + ch.size.y) / (float)atlas->size.y},
        };

        if (textInfo.useColorGradient) {
            tr.color = textInfo.colorGradient.topRightColor;
            tl.color = textInfo.colorGradient.topLeftColor;
            bl.color = textInfo.colorGradient.bottomLeftColor; 
            br.color = textInfo.colorGradient.bottomRightColor; 
        }
        else {
            bl.color = textInfo.color;
            br.color = textInfo.color;
            tl.color = textInfo.color;
            tr.color = textInfo.color;
        }

        TextBatch::AddCharacter(bl, br, tl, tr);

        x += (ch.advance.x >> 6) * scale.x;  // Bitshift by 6 to get a value in pixels (1/64th times 2^6 = 64)
        y += (ch.advance.y >> 6) * scale.y; 
    }
    TextBatch::Flush();
    glDisable(GL_BLEND);
}

void DebugTextInfo(const std::string& label, TextInfo& textInfo) {
    ImGui::Begin(label.c_str());
    ImGui::ColorEdit3("color", glm::value_ptr(textInfo.color));
    ImGui::Checkbox("color gradient?", &textInfo.useColorGradient);
    ImGui::ColorEdit3("color tr", glm::value_ptr(textInfo.colorGradient.topRightColor));
    ImGui::ColorEdit3("color tl", glm::value_ptr(textInfo.colorGradient.topLeftColor));
    ImGui::ColorEdit3("color bl", glm::value_ptr(textInfo.colorGradient.bottomLeftColor));
    ImGui::ColorEdit3("color br", glm::value_ptr(textInfo.colorGradient.bottomRightColor));
    ImGui::InputFloat("width", &textInfo.width);
    ImGui::InputFloat("edge", &textInfo.edge);
    ImGui::InputFloat("borderWidth", &textInfo.borderWidth);
    ImGui::InputFloat("borderEdge", &textInfo.borderEdge);
    ImGui::InputFloat2("borderOffset", glm::value_ptr(textInfo.borderOffset));
    ImGui::ColorEdit3("borderColor", glm::value_ptr(textInfo.outlineColor));
    ImGui::End();
}