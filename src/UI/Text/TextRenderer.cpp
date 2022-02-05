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

void TextRenderer::RenderText(const std::string& text, float size, const glm::vec2& position, const TextInfo& textInfo, const Font& font) {
    auto shader {AssetManager::GetShader("text")};
    if (font.mode == FontRenderMode::SDF) {
        shader = AssetManager::GetShader("textSDF");
    }
    shader->Use();

    Atlas* atlas;

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

        shader->SetFloat("textInfo.width", textInfo.width);
        shader->SetFloat("textInfo.edge", textInfo.edge);
        shader->SetFloat("textInfo.borderWidth", textInfo.borderWidth);
        shader->SetFloat("textInfo.borderEdge", textInfo.borderEdge);
        shader->SetVec2("textInfo.borderOffset", textInfo.borderOffset);
        shader->SetVec3("textInfo.outlineColor", Color2Vec3(textInfo.outlineColor));
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
                penx += textInfo.wordSpacing + (atlas->metricsWidth / 2 >> 6) * scale.x;
                continue;
            case '\n':
                penx = position.x;
                peny += textInfo.lineSpacing + (atlas->metricsHeight >> 6) * scale.y;
                continue;
            case '\t':
                penx += (textInfo.wordSpacing + (atlas->metricsWidth / 2 >> 6) * scale.x) * textInfo.tabSpaces;
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

        if (textInfo.useColorGradient) {
            tr.color = Color2Vec4(textInfo.colorGradient.topRightColor);
            tl.color = Color2Vec4(textInfo.colorGradient.topLeftColor);
            bl.color = Color2Vec4(textInfo.colorGradient.bottomLeftColor); 
            br.color = Color2Vec4(textInfo.colorGradient.bottomRightColor); 
        }
        else {
            bl.color = Color2Vec4(textInfo.color);
            br.color = Color2Vec4(textInfo.color);
            tl.color = Color2Vec4(textInfo.color);
            tr.color = Color2Vec4(textInfo.color);
        }

        TextBatch::AddCharacter(bl, br, tl, tr);

        penx += textInfo.letterSpacing + (ch.advance.x >> 6) * scale.x;  // Bitshift by 6 to get a value in pixels (1/64th times 2^6 = 64)
        peny += (ch.advance.y >> 6) * scale.y; 
    }
    TextBatch::Flush();
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void DebugTextInfoWindow(const std::string& label, TextInfo& textInfo) {
    ImGui::Begin(label.c_str());
    DebugColorRGB("color", textInfo.color);
    ImGui::Checkbox("color gradient?", &textInfo.useColorGradient);
    DebugColorRGB("color tl", textInfo.colorGradient.topLeftColor);
    DebugColorRGB("color tr", textInfo.colorGradient.topRightColor);
    DebugColorRGB("color bl", textInfo.colorGradient.bottomLeftColor);
    DebugColorRGB("color br", textInfo.colorGradient.bottomRightColor);
    ImGui::InputFloat("width", &textInfo.width);
    ImGui::InputFloat("edge", &textInfo.edge);
    ImGui::InputFloat("borderWidth", &textInfo.borderWidth);
    ImGui::InputFloat("borderEdge", &textInfo.borderEdge);
    ImGui::InputFloat2("borderOffset", glm::value_ptr(textInfo.borderOffset));
    DebugColorRGB("borderColor", textInfo.outlineColor);
    ImGui::InputFloat("letterSpacing", &textInfo.letterSpacing);
    ImGui::InputFloat("wordSpacing", &textInfo.wordSpacing);
    ImGui::InputFloat("lineSpacing", &textInfo.lineSpacing);
    ImGui::InputInt("tabSpaces", &textInfo.tabSpaces);
    ImGui::End();
}