#include "Scene.hpp"

#include "AssetsManager.hpp"
#include "Components.hpp"
#include "Engine.hpp"
#include "GameObject.hpp"
#include "Log.hpp"
#include "Time.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"

#include <algorithm>

Scene::Scene(Engine* engine) : engine{engine} {
    if (!haveObjectsStarted) {
        haveObjectsStarted = true;
        for (auto& gameobject : gameobjects) {
            if (gameobject->isActive && !gameobject->hasStarted) {
                gameobject->hasStarted = true;
                gameobject->Start();
                gameobject->OnEnable();
            }
        }
    }
}

Scene::~Scene() {
   
}

void Scene::Update() {
    //! Update animations
    for (auto&& [entity, animator, sprite] : entityRegistry.view<Animator, SpriteRenderer>().each()) {
        if (animator.frames.empty())
            continue;

        animator.timer += Time::deltaTime;
        if (animator.timer >= animator.frames[animator.currentFrame].duration) {
            ++animator.currentFrame;
            animator.timer = 0;
            if (animator.currentFrame >= animator.frames.size()) 
                animator.currentFrame = 0;
            sprite.sprite->SetTexture(animator.frames[animator.currentFrame].texture);
        }
    } 
    for (auto&& [entity, animator, tilemap] : entityRegistry.view<Animator, TilemapRenderer>().each()) {
        if (animator.frames.empty())
            continue;

        animator.timer += Time::deltaTime;
        if (animator.timer >= animator.frames[animator.currentFrame].duration) {
            ++animator.currentFrame;
            animator.timer = 0;
            if (animator.currentFrame >= animator.frames.size())
                animator.currentFrame = 0;
            tilemap.SetTextureAtlas(animator.frames[animator.currentFrame].texture);
        }
    } 

    UpdateGameObjects();

    // Delete destroyed gameobjects
    if (isAnyGameObjectDead) {
        isAnyGameObjectDead = false;
        gameobjects.erase(std::remove_if(gameobjects.begin(), gameobjects.end(), 
            [](Owned<GameObject>& go) { 
                return !go->isAlive;
            }
        ), gameobjects.end());
    }
}

void Scene::Render() {
    //! First update model matrices for all gameobjects
    for (auto&& [entity, transform] : entityRegistry.view<Transform>().each()) {
        transform.UpdateTransform();
    }

    //! Render tilemaps
    entityRegistry.sort<TilemapRenderer>([](const TilemapRenderer& a, const TilemapRenderer& b) {
        return a.GetLayer() < b.GetLayer();
    });
    entityRegistry.sort<Transform, TilemapRenderer>();  //+ Also sort Transform in order to reduce cache misses
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    auto tilemapShader {AssetsManager::GetShader("tilemap")};
    tilemapShader->Use();
    for (auto&& [entity, tilemap, transform] : entityRegistry.view<TilemapRenderer, Transform>().each()) {
        if (!tilemap.IsConstructed())
            continue;

        // Update tilemap buffer data to gpu
        tilemap.UpdateBufferData();
        tilemapShader->SetIVec2("mapSize", glm::ivec2{tilemap.GetSize().x, tilemap.GetSize().y});
        tilemapShader->SetMatrix4("model", transform.GetModel());
        tilemapShader->SetInt("tileSize", tilemap.GetTileSize());
        tilemapShader->SetIVec2("atlasTexSize", tilemap.GetAtlasTexSize());
        tilemap.GetTextureAtlas()->Use();
        tilemap.GetMesh()->Use();
        tilemap.GetMesh()->Draw();
    }

    //! Render sprites
    // TODO: Move this to a system somewhere else?
    entityRegistry.sort<SpriteRenderer>([](const SpriteRenderer& a, const SpriteRenderer& b) {
        return a.renderOrder < b.renderOrder;
    });
    entityRegistry.sort<Transform, SpriteRenderer>(); //+ Also sort Transform in order to reduce cache misses
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_BLEND);
    auto spriteShader{AssetsManager::GetShader("sprite")};
    spriteShader->Use();
    auto spriteVAO{AssetsManager::GetVertexArray("sprite")};
    spriteVAO->Use();
    Texture* activeTexture {};
    for (auto&& [entity, sprite, transform] : entityRegistry.view<SpriteRenderer, Transform>().each()) {
        if (!activeTexture || sprite.sprite->GetTexture()->GetID() != activeTexture->GetID()) {
            activeTexture = sprite.sprite->GetTexture().get();
            activeTexture->Use();
        } 
        spriteShader->SetMatrix4("model", transform.GetModel());
        spriteShader->SetVec2("spriteMinUV", sprite.sprite->GetMinUV());
        spriteShader->SetVec2("spriteMaxUV", sprite.sprite->GetMaxUV());
        spriteShader->SetIVec2("spriteSize", sprite.sprite->GetSize());
        spriteShader->SetVec4("color", sprite.color);
        spriteVAO->Draw();
    }
    glDisable(GL_BLEND);

    //! Render UI
}

void Scene::UpdateGameObjects() {
    for (auto& gameobject : gameobjects) {
        if (gameobject->isActive && gameobject->isAlive) {
            if (!gameobject->hasStarted) {
                gameobject->hasStarted = true;
                gameobject->Start();
                gameobject->OnEnable();
            }

            gameobject->Update();
        }
    }
}

GameObject* Scene::AddGameObject(Owned<GameObject> gameobject) {
    gameobjects.emplace_back(std::move(gameobject));
    return gameobjects[gameobjects.size() - 1].get();
}