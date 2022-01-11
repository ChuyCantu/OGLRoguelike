#include "Scene.hpp"

#include "AssetsManager.hpp"
#include "Components.hpp"
#include "Engine.hpp"
#include "GameObject.hpp"
#include "Log.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"

#include <algorithm>

Scene::Scene(Engine* engine) : engine{engine} {
    Load();

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
    UpdateGameObjects();

    // Delete destroyed gameobjects
    if (isAnyGameObjectDead) {
        isAnyGameObjectDead = false;
        gameobjects.erase(std::remove_if(gameobjects.begin(), gameobjects.end(), 
            [](Owned<GameObject>& go) { 
                return !go->isAlive;
            }
        ));
    }
}

void Scene::Render() {
    //! First update model matrices for all gameobjects
    for (auto&& [entity, transform] : entityRegistry.view<Transform>().each()) {
        transform.UpdateTransform();
    }

    //! Render tilemaps
    

    //! Render sprites
    // TODO: Move this to a system somewhere else?
    entityRegistry.sort<SpriteRenderer>([](const SpriteRenderer& a, const SpriteRenderer& b) {
        return a.renderOrder < b.renderOrder;
    });
    // entityRegistry.sort<Transform, SpriteRenderer>(); //+ Also sort Transform in order to reduce cache misses
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
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