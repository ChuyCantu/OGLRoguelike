#include "Image.hpp"

#include "Core/AssetManager.hpp"
#include "Rendering/Batch.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/VertexArray.hpp"

#include <glm/gtc/matrix_transform.hpp>

Image::Image(const std::string& name) : Widget{name} {
    ignoreInput = true;
}

Image::Image(const Rect& rect, Ref<Sprite> sprite, const std::string& name) : Widget{rect, name}, sprite{sprite} {
    ignoreInput = true;
}

Image::Image(const Rect& rect, const std::vector<Ref<class Sprite>>& sprites, const std::string& name) : Widget{rect, name}, useNineSlice{true}, slicedSprites{sprites} {
    ignoreInput = true;
}

Image::Image(const Rect& rect, const std::vector<Ref<class Sprite>>&& sprites, const std::string& name) : Widget{rect, name}, useNineSlice{true}, slicedSprites{std::move(sprites)} {
    ignoreInput = true;
}

void Image::Draw() {
#ifdef GUI_BATCH
    auto guiShader {AssetManager::GetShader("gui")};
#else
    auto uiShader{AssetManager::GetShader("guiOld")};
    uiShader->Use();
#endif  // GUI_BATCH

    if (!useNineSlice) {
        UpdateTransform();
#ifdef GUI_BATCH
        if (!SpriteBatch::IsReadyForRendering())
            SpriteBatch::Start();
        SpriteBatch::DrawGUISprite(rect, glm::vec3{1.0f, 1.0f, 1.0f}, sprite.get(), color, GetModel(), guiShader.get());
#else
        sprite->GetTexture()->Use();
        uiShader->SetMatrix4("model", GetModel());
        uiShader->SetVec2("spriteMinUV", sprite->GetMinUV());
        uiShader->SetVec2("spriteMaxUV", sprite->GetMaxUV());
        uiShader->SetVec4("color", Color2Vec4(color));
        uiShader->SetBool("flipX", sprite->flipX);
        uiShader->SetBool("flipY", sprite->flipY);
        uiShader->SetBool("useVirtualResolution", true);
        auto guiVao{AssetManager::GetVertexArray("gui")};
        guiVao->Use();
        guiVao->Draw();
#endif  // GUI_BATCH
    }
    else {
        slicedSprites[0]->GetTexture()->Use();
#ifndef GUI_BATCH   
        auto guiVao {AssetManager::GetVertexArray("gui")};

        uiShader->SetVec4("color", Color2Vec4(color));
        uiShader->SetBool("useVirtualResolution", true);
#endif // GUI_BATCH
        
        glm::mat4 newModel; 
        glm::vec3 newScale;

        for (int i {0}; i < slicedSprites.size(); ++i) {
            glm::vec2 currentSpriteSize {glm::vec2{static_cast<float>(slicedSprites[0]->GetSize().x), static_cast<float>(slicedSprites[0]->GetSize().y)}};

            switch (i) {
            case 0: // TL
                newModel = glm::translate(glm::mat4{1.0}, glm::vec3{rect.position.x, rect.position.y, 0.0f});
                newScale = glm::vec3{currentSpriteSize.x, currentSpriteSize.y, 1.0f};
                break;
            case 1: // TC
                newModel = glm::translate(glm::mat4{1.0}, glm::vec3{rect.position.x + slicedSprites[0]->GetSize().x, rect.position.y, 0.0f});
                newScale = glm::vec3{rect.size.x - slicedSprites[0]->GetSize().x - slicedSprites[2]->GetSize().x, currentSpriteSize.y, 1.0f};
                break;
            case 2: // TR
                newModel = glm::translate(glm::mat4{1.0}, glm::vec3{rect.position.x + rect.size.x - currentSpriteSize.x, rect.position.y, 0.0f});
                newScale = glm::vec3{currentSpriteSize.x, currentSpriteSize.y, 1.0f};
                break;
            case 3: // CL
                newModel = glm::translate(glm::mat4{1.0}, glm::vec3{rect.position.x, rect.position.y + slicedSprites[0]->GetSize().y, 0.0f});
                newScale = glm::vec3{currentSpriteSize.x, rect.size.y - slicedSprites[0]->GetSize().y - slicedSprites[6]->GetSize().y, 1.0f};
                break;
            case 4: // C
                newModel = glm::translate(glm::mat4{1.0}, glm::vec3{rect.position.x + slicedSprites[3]->GetSize().x, rect.position.y + slicedSprites[0]->GetSize().y, 0.0f});
                newScale = glm::vec3{rect.size.x - slicedSprites[3]->GetSize().x - slicedSprites[5]->GetSize().x,
                                     rect.size.y - slicedSprites[1]->GetSize().y - slicedSprites[7]->GetSize().y, 1.0f};
                break;
            case 5: // CR
                newModel = glm::translate(glm::mat4{1.0}, glm::vec3{rect.position.x + rect.size.x - currentSpriteSize.x, rect.position.y + slicedSprites[2]->GetSize().y, 0.0f});
                newScale = glm::vec3{currentSpriteSize.x, rect.size.y - slicedSprites[2]->GetSize().y - slicedSprites[8]->GetSize().x, 1.0f};
                break;
            case 6: // BL
                newModel = glm::translate(glm::mat4{1.0}, glm::vec3{rect.position.x, rect.position.y + rect.size.y - currentSpriteSize.y, 0.0f});
                newScale = glm::vec3{currentSpriteSize.x, currentSpriteSize.y, 1.0f};
                break;
            case 7: // BC
                newModel = glm::translate(glm::mat4{1.0}, glm::vec3{rect.position.x + slicedSprites[6]->GetSize().x, rect.position.y + rect.size.y - currentSpriteSize.y, 0.0f});
                newScale = glm::vec3{rect.size.x - slicedSprites[6]->GetSize().x - slicedSprites[8]->GetSize().x, currentSpriteSize.y, 1.0f};
                break;
            case 8: // BR
                newModel = glm::translate(glm::mat4{1.0}, glm::vec3{rect.position.x + rect.size.x - currentSpriteSize.x, rect.position.y + rect.size.y - currentSpriteSize.y, 0.0f});
                newScale = glm::vec3{currentSpriteSize.x, currentSpriteSize.y, 1.0f};
                break;

            default:
                return;
            }
            newModel = glm::scale(newModel, newScale);

#ifdef GUI_BATCH
            if (!SpriteBatch::IsReadyForRendering())
                SpriteBatch::Start();
            SpriteBatch::DrawGUISprite(rect, newScale, slicedSprites[i].get(), color, newModel, guiShader.get());
#else
            uiShader->SetMatrix4("model", newModel);
            uiShader->SetVec2("spriteMinUV", slicedSprites[i]->GetMinUV());
            uiShader->SetVec2("spriteMaxUV", slicedSprites[i]->GetMaxUV());
            uiShader->SetBool("flipX", slicedSprites[i]->flipX);
            uiShader->SetBool("flipY", slicedSprites[i]->flipY);
            guiVao->Use();
            guiVao->Draw();
#endif  // GUI_BATCH
        }
    }
}