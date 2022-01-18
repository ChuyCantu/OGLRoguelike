#include "Scene.hpp"

#include "AssetManager.hpp"
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
   
}

Scene::~Scene() {
   
}

void Scene::Update() {
    //! Call Start() after all GameObjects have been initialized
    if (firstLoop) {
        for (auto& gameobject : gameobjects) {
            if (gameobject->isActive && !gameobject->hasStarted) {
                gameobject->hasStarted = true;
                gameobject->Start();
                gameobject->OnEnable();
            }
        }
    }

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

    //! Update gameobjects
    UpdateGameObjects();

    //! Check for collisions (this is very basic, for direct tile movement and may change in the future)
    // TODO: Replace this with commands when turns are implemented, so the position is managed by the command and not manually
    if (!firstLoop) {
        for (auto&& [entity1, transform1, collider1] : entityRegistry.view<Transform, Collider>().each()) {
            for (auto&& [entity2, transform2, collider2] : entityRegistry.view<Transform, Collider>().each()) {
                if (entity1 == entity2)
                    continue;

                //+ For now this only works for movement with steps of size tileSize!!!
                if (transform1.GetPosition() == transform2.GetPosition()) { // Collided!
                    if (transform1.GetPosition() == collider1.prevPosition) {
                        transform1.gameobject->OnCollisionStay(collider2);
                        transform2.gameobject->OnCollisionStay(collider1);
                    }
                    else {
                        if (!collider2.canPassThrough) {
                            transform1.SetPosition(collider1.prevPosition);
                        }
                        transform1.gameobject->OnCollisionEnter(collider2);
                        transform2.gameobject->OnCollisionEnter(collider1);
                    }
                }
                else if (collider1.prevPosition == transform2.GetPosition()) {
                        transform1.gameobject->OnCollisionExit(collider2);
                        transform2.gameobject->OnCollisionExit(collider1);
                }
            }
            for (auto&& [entity3, tilemap, tmCollider] : entityRegistry.view<TilemapRenderer, TilemapCollider>().each()) {
                const glm::vec3& pos1{transform1.GetPosition()};
                //! This is only working when tilemap is at pos 0!
                // TODO: Make change the coordinates to be relative to the tilemap and not to the world
                if (tilemap.GetTile(static_cast<int>(pos1.x) / tilemap.GetTileSize(), static_cast<int>(pos1.y) / tilemap.GetTileSize()) != 0) {  // Collided
                    if (transform1.GetPosition() == collider1.prevPosition) {
                        Collider tilemapCollider;
                        tilemapCollider.gameobject = tmCollider.gameobject;
                        tilemapCollider.canPassThrough = tmCollider.canPassThrough;
                        collider1.gameobject->OnCollisionStay(tilemapCollider);
                    } else {
                        if (!tmCollider.canPassThrough) {
                            transform1.SetPosition(collider1.prevPosition);
                        }
                        // Make sure OnCollisionEnter is triggered by the whole tilemap collider and not by each tile (preventing calling it when moving from tile to tile within the tilemap):
                        if (tilemap.GetTile(static_cast<int>(collider1.prevPosition.x) / tilemap.GetTileSize(), static_cast<int>(collider1.prevPosition.y) / tilemap.GetTileSize()) == 0) {
                            Collider tilemapCollider;
                            tilemapCollider.gameobject = tmCollider.gameobject;
                            tilemapCollider.canPassThrough = tmCollider.canPassThrough;
                            collider1.gameobject->OnCollisionEnter(tilemapCollider);
                        }
                    }
                } else if (tilemap.GetTile(static_cast<int>(collider1.prevPosition.x) / tilemap.GetTileSize(), static_cast<int>(collider1.prevPosition.y) / tilemap.GetTileSize()) != 0) {
                    Collider tilemapCollider;
                    tilemapCollider.gameobject = tmCollider.gameobject;
                    tilemapCollider.canPassThrough = tmCollider.canPassThrough;
                    collider1.gameobject->OnCollisionExit(tilemapCollider);
                }
            }
        }
    }
    
    //! Update previous position cache on the collider
    for (auto&& [entity, transform, collider] : entityRegistry.view<Transform, Collider>().each()) {
        collider.prevPosition = transform.GetPosition();
    }

    //! Delete destroyed gameobjects
    if (isAnyGameObjectDead) {
        isAnyGameObjectDead = false;
        gameobjects.erase(std::remove_if(gameobjects.begin(), gameobjects.end(), 
            [](Owned<GameObject>& go) { 
                return !go->isAlive;
            }
        ), gameobjects.end());
    }

    if (firstLoop) {
        firstLoop = false;
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
    glEnable(GL_CULL_FACE); //! Face culling only tilemaps since sprites can swap x scale to flip around (and for optimizing tilemap rendering)
    auto tilemapShader {AssetManager::GetShader("tilemap")};
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
    glDisable(GL_CULL_FACE);

    //! Render sprites
    entityRegistry.sort<SpriteRenderer>([](const SpriteRenderer& a, const SpriteRenderer& b) {
        return a.renderOrder < b.renderOrder;
        // TODO: Add texture id check so it is also considered but with less impact than render order
    });
    entityRegistry.sort<Transform, SpriteRenderer>(); //+ Also sort Transform in order to reduce cache misses
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_BLEND);
    auto spriteShader{AssetManager::GetShader("sprite")};
    spriteShader->Use();
    auto spriteVAO{AssetManager::GetVertexArray("sprite")};
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
        spriteShader->SetBool("flip", sprite.flip);
        spriteVAO->Draw();
    }
    glDisable(GL_BLEND);

    //! Render UI
    // TODO
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
    return gameobjects.emplace_back(std::move(gameobject)).get();
}