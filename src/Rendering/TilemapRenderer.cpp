#include "TilemapRenderer.hpp"

#include "Camera.hpp"
#include "Core/AssetsManager.hpp"
#include "Core/Time.hpp"
#include "Core/Log.hpp"
#include "Input/Input.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Utils/Random.hpp"
#include "Utils/MathExtras.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <entt/entity/registry.hpp>
#include <glm/gtx/quaternion.hpp>

TilemapRenderer::TilemapRenderer(int w, int h, int tileSize) 
    : width{w}, height{h}, tileSize{tileSize}, tiles(w * h) {

#ifdef ANIM_EXAMPLE
    AssetsManager::AddShader("tilemapv3", "resources/shaders/tilemapv3.glsl");

    for (uint16_t i{0}; i < tiles.size(); ++i) {
        tiles[i] = Random::Range(208, 210);
        // tiles[i] = Random::Range(0, 209);
    }

    VertexLayout layout{
        VertexElement{1, DataType::UShort, true}};
    vao = VertexArray{tiles.data(), static_cast<uint32_t>(tiles.size()), layout, BufferUsage::Dynamic};
    vao.SetDrawMode(DrawMode::Points);

    auto texture{AssetsManager::AddTexture("Pit0", MakeRef<Texture>("resources/assets/Pit0.png", true))};
    texture->SetMinFilter(TextureParameter::Nearest).SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);
    textures.push_back(texture);
    texture = AssetsManager::AddTexture("Pit1", MakeRef<Texture>("resources/assets/Pit1.png", true));
    texture->SetMinFilter(TextureParameter::Nearest).SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);
    textures.push_back(texture);
    activeTexture = 0;
#else
    // shader.Load("resources/shaders/tilemap.glsl");
    AssetsManager::AddShader("tilemap", "resources/shaders/tilemap.glsl");
    AssetsManager::AddShader("tilemapv3", "resources/shaders/tilemapv3.glsl");

    for (uint16_t i {0}; i < tiles.size(); ++i) {
        tiles[i] = i; //Random::Range(0, 1919);
    }

    VertexLayout layout {
        VertexElement {1, DataType::UShort, true}
    };
    vao = VertexArray{tiles.data(), static_cast<uint32_t>(tiles.size()), layout, BufferUsage::Dynamic};
    vao.SetDrawMode(DrawMode::Points);

    // texture.Load("resources/assets/TileSetSample.png", true);
    // texture.Load("resources/assets/Font.png", true);
    // texture.Load("resources/assets/Custom2_16x16.png", true);
    // texture.SetMinFilter(TextureParameter::Nearest).SetMagFilter(TextureParameter::Nearest)
    //        .SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);

    auto texture{AssetsManager::AddTexture("tilemap", MakeRef<Texture>("resources/assets/ignore/ASCII Roguelike/16x16.bmp", true))};
    texture->SetMinFilter(TextureParameter::Nearest).SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);
#endif  // ANIM_EXAMPLE
}

glm::vec3 tilemapPos{-310.f, -120.f, 0.f};
void TilemapRenderer::Draw() 
{
    //! This is only for debuging and it's temporary
    auto& camera{Camera::GetMainCamera()};
    glm::vec3 cameraPos{camera.GetPosition()};

    // auto& input = Input::system->GetState().Keyboard;
    //+ Movement is reversed for the camera
    if (Input::GetKey(SDL_SCANCODE_D)/*input.GetKeyValue(SDL_SCANCODE_D)*/) {
        cameraPos.x += 1.f;
    }
    if (Input::GetKey(SDL_SCANCODE_A)/*input.GetKeyValue(SDL_SCANCODE_A)*/) {
        cameraPos.x -= 1.f;
    // if (input.GetKeyState(SDL_SCANCODE_A) == ButtonState::Pressed) {
    //    cameraPos.x -= 16.f;
    }
    if (Input::GetKey(SDL_SCANCODE_W)/*input.GetKeyValue(SDL_SCANCODE_W)*/) {
        cameraPos.y += 1.f;
    }
    if (Input::GetKey(SDL_SCANCODE_S)/*input.GetKeyValue(SDL_SCANCODE_S)*/) {
        cameraPos.y -= 1.f;
    }
    camera.SetPosition(cameraPos);
    float scroll { Input::GetMouseWheel().y}; // Input::system->GetState().Mouse.GetScrollValue().y};
    if (scroll != 0)
        camera.SetScale(camera.GetScale() + scroll * 0.02);

    // Input::SetMouseRelativeMode(true);
    // auto& mpos {Input::GetMousePosition()};
    // LOG_TRACE("Mouse Pos: {}, {}", mpos.x, mpos.y);

    // if (Input::system->GetState().Mouse.GetButtonState(SDL_BUTTON_LEFT) == ButtonState::Pressed) {
    //     auto& mousePos{Input::system->GetState().Mouse.GetPosition()};
    //     camera.ScreenToWorld2D(mousePos);
    // }
    // if (Input::system->GetState().Mouse.GetButtonState(SDL_BUTTON_RIGHT) == ButtonState::Pressed) {
    //     auto& mousePos{Input::system->GetState().Mouse.GetPosition()};
    //     // camera.World2DToVirtualScreen(glm::vec2{0.f});
    //     LOG_TRACE("InCamera: {}", camera.IsPointInside(camera.World2DToVirtualScreen(glm::vec2{0.f})));
    // }

    float cspeed {1.f};
    if (Input::GetKey(SDL_SCANCODE_LEFT)/*input.GetKeyValue(SDL_SCANCODE_LEFT)*/) {
        tilemapPos.x -= cspeed ; //* Time::deltaTime;
    }
    if (Input::GetKey(SDL_SCANCODE_RIGHT)/*input.GetKeyValue(SDL_SCANCODE_RIGHT)*/) {
        tilemapPos.x += cspeed ; //* Time::deltaTime;
        // if (input.GetKeyState(SDL_SCANCODE_A) == ButtonState::Pressed) {
        //    tilemapPos.x -= 16.f;
    }
    if (Input::GetKey(SDL_SCANCODE_UP)/*input.GetKeyValue(SDL_SCANCODE_UP)*/) {
        tilemapPos.y += cspeed ; //* Time::deltaTime;
    }
    if (Input::GetKey(SDL_SCANCODE_DOWN)/*input.GetKeyValue(SDL_SCANCODE_DOWN)*/) {
        tilemapPos.y -= cspeed ; //* Time::deltaTime;
    }
    if (Input::GetKeyDown(SDL_SCANCODE_K)/*input.GetKeyState(SDL_SCANCODE_K) == ButtonState::Pressed*/) {
        DebugChangeSomeTiles();
    }
    //! ==================================================================

    // TODO: Fix shader drawing backwards
    // glCullFace(GL_BACK);
    // glEnable(GL_CULL_FACE);
#ifdef ANIM_EXAMPLE
    if (Input::GetKeyDown(SDL_SCANCODE_KP_5)) {// input.GetKeyState(SDL_SCANCODE_KP_5) == ButtonState::Pressed) {
        activeTexture = activeTexture == 0 ? 1 : 0;
    }

    // static float rotation {5.f};
    Ref<Shader> shader{AssetsManager::GetShader("tilemapv3")};
    shader->Use();
    shader->SetIVec2("mapSize", glm::ivec2{width, height});
    glm::mat4 model{1.0f};
    // model = glm::translate(model, tilemapPos);
    // model = glm::rotate(model, glm::radians(30.f), glm::vec3{0.f, 0.f, 1.f});
    // rotation += 15.f * Time::deltaTime;
    
    //+ Rotating around a pivot, since tilemap and sprite pivots are in the lower left corner and not in the center
    //+ Function in MathExtras
    glm::quat rot {glm::angleAxis(glm::radians(0.f), glm::vec3{0.0f, 0.0f, 1.f})};
    glm::vec3 pos {rot * (tilemapPos - (tilemapPos + glm::vec3{80.f, 80.f, 0.f})) + (tilemapPos + glm::vec3{80.f, 80.f, 0.f})};
    rot = rot * quaternion::identity;
    glm::mat4 translation {glm::translate(glm::mat4{1.0}, pos)};
    glm::mat4 rotation = glm::mat4_cast(rot);
    glm::mat4 scale = glm::scale(glm::mat4{1.0f}, glm::vec3{1.0});
    model = translation * rotation * scale;
    
    shader->SetMatrix4("model", model);
    shader->SetInt("tileSize", tileSize);

    // TODO: Calculate this based on texture size and tileSize (a reference to a texture must be included)
    shader->SetIVec2("atlasTexSize", glm::ivec2{8, 32});

    textures[activeTexture]->Use();
    vao.Use();
    vao.Draw();
#else
    Ref<Shader> shader {AssetsManager::GetShader("tilemapv3")};
    shader->Use();
    // AssetsManager::GetShader("tilemap")->SetVec2("mapSize", glm::vec2{width, height});
    shader->SetIVec2("mapSize", glm::ivec2{width, height});
    glm::mat4 model{1.0f};
    model = glm::translate(model, tilemapPos);
    shader->SetMatrix4("model", model);
    shader->SetInt("tileSize", tileSize);
    // shader->SetIVec2("atlasTexSize", glm::ivec2{16, 16});

    // TODO: Calculate this based on texture size and tileSize (a reference to a texture must be included)
    shader->SetIVec2("atlasTexSize", glm::ivec2{32, 60});

    AssetsManager::GetTexture("tilemap")->Use(0);
    vao.Use();
    vao.Draw();
#endif  // ANIM_EXAMPLE
    // glDisable(GL_CULL_FACE);
}

void TilemapRenderer::DebugChangeSomeTiles() {
    std::vector<uint8_t> data(tiles.size());
    for (int i {0}; i < data.size(); ++i) {
        data[i] = Random::Range(0, width * height - 1);
    }
    vao.GetVertexBuffer().SetData(0, data.size(), data.data());
}