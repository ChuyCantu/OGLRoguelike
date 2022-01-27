#include "Image.hpp"

#include "Core/AssetManager.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/VertexArray.hpp"

Image::Image() : Widget{} {

}

Image::Image(const Rect& rect, Ref<Sprite> sprite) : Widget{rect}, sprite{sprite} {

}

void Image::Draw() {
    auto uiShader{AssetManager::GetShader("gui")};
    sprite->GetTexture()->Use();

    UpdateTransform();
    uiShader->SetMatrix4("model", GetModel());
    uiShader->SetVec2("spriteMinUV", sprite->GetMinUV());
    uiShader->SetVec2("spriteMaxUV", sprite->GetMaxUV());
    uiShader->SetVec4("color", glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
    uiShader->SetBool("flip", false);
    uiShader->SetBool("useVirtualResolution", true);
    AssetManager::GetVertexArray("sprite")->Draw();
}