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

#include "UI/Panel.hpp"

#include <algorithm>

// TODO: If game is closed while a component is being retrived by the entt system, it will crash (e.i. if (Input::GetKey(key) {go.GetComponent<T>()...} ))

Scene::Scene(Engine* engine) : engine{engine} {
    // entityRegistry.on_construct<TilemapRenderer>().connect<&OnTilemapAdded>();
}

Scene::~Scene() {
   
}

// TODO: Add IsActive checks on EVERYTHING in here
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

    //! Check for collisions (Basic collision detection based on position, this can be changed to actual collision detection if needed)
    if (!firstLoop) {
        for (auto&& [entityA, transformA, moveA, colliderA] : entityRegistry.view<Transform, MoveComponent, Collider>().each()) {
            for (auto&& [entityB, transformB, moveB, colliderB] : entityRegistry.view<Transform, MoveComponent, Collider>().each()) {
                if (entityA == entityB) 
                    continue;

                if (moveA.GetDestPosition() == transformB.GetPosition()) { // Collision!
                    if (moveA.startedMove) {
                        if (colliderB.isSolid && !colliderA.ignoreSolid)
                            moveA.Cancel();
                        transformA.gameobject->OnCollisionEnter(colliderB);
                        // transformB.gameobject->OnCollisionEnter(colliderA);
                    }
                    else if (!moveA.IsMoving()) {
                        transformA.gameobject->OnCollisionStay(colliderB);
                        // transformB.gameobject->OnCollisionStay(colliderA);
                    } 
                } 
                else if (moveA.startedMove && moveA.GetSrcPosition() == transformB.GetPosition()) {
                    transformA.gameobject->OnCollisionExit(colliderB);
                    // transformB.gameobject->OnCollisionExit(colliderA);
                }
            }
            for (auto&& [entityC, tilemap, tilemapCollider] : entityRegistry.view<TilemapRenderer, TilemapCollider>().each()) {
                glm::vec3 tilemapPos{tilemap.gameobject->GetComponent<Transform>().GetPosition() / (float)tilemap.GetTileSize()};
                glm::ivec2 tilePos{static_cast<int>(moveA.GetDestPosition().x) / tilemap.GetTileSize(), 
                                   static_cast<int>(moveA.GetDestPosition().y) / tilemap.GetTileSize()};
                if (tilemap.GetTile(tilePos.x - tilemapPos.x, tilePos.y - tilemapPos.y) != 0) {  // Collided
                    if (!moveA.IsMoving()) {
                        Collider newCollider;
                        newCollider.gameobject = tilemapCollider.gameobject;
                        newCollider.isSolid = tilemapCollider.isSolid;
                        colliderA.gameobject->OnCollisionStay(newCollider);
                    } 
                    else if (moveA.startedMove) {
                        if (tilemapCollider.isSolid && !colliderA.ignoreSolid)
                            moveA.Cancel();

                        //* Make sure OnCollisionEnter is triggered by the whole tilemap collider and not by each tile (preventing calling it when moving from tile to tile within the tilemap):
                        glm::vec3 tilemapPos{tilemap.gameobject->GetComponent<Transform>().GetPosition() / (float)tilemap.GetTileSize()};
                        glm::ivec2 tilePrevPos{static_cast<int>(moveA.GetSrcPosition().x) / tilemap.GetTileSize(),
                                               static_cast<int>(moveA.GetSrcPosition().y) / tilemap.GetTileSize()};
                        if (tilemap.GetTile(tilePrevPos.x - tilemapPos.x, tilePrevPos.y - tilemapPos.y) == 0) {
                            Collider newCollider;
                            newCollider.gameobject = tilemapCollider.gameobject;
                            newCollider.isSolid = tilemapCollider.isSolid;
                            colliderA.gameobject->OnCollisionEnter(newCollider);
                        }
                    }
                } else {
                    glm::vec3 tilemapPos{tilemap.gameobject->GetComponent<Transform>().GetPosition() / (float)tilemap.GetTileSize()};
                    glm::ivec2 tilePrevPos{static_cast<int>(moveA.GetSrcPosition().x) / tilemap.GetTileSize(),
                                           static_cast<int>(moveA.GetSrcPosition().y) / tilemap.GetTileSize()};
                    if (moveA.startedMove && tilemap.GetTile(tilePrevPos.x - tilemapPos.x, tilePrevPos.y - tilemapPos.y) != 0) {
                        Collider newCollider;
                        newCollider.gameobject = tilemapCollider.gameobject;
                        newCollider.isSolid = tilemapCollider.isSolid;
                        colliderA.gameobject->OnCollisionExit(newCollider);
                    }
                }
            }
        }
    }

    //! Move entities
    for (auto&& [entity, transform, moveComponent] : entityRegistry.view<Transform, MoveComponent>().each()) {
        if (moveComponent.IsMoving())
            moveComponent.Update();
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

    LastUpdate();

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
    // glDisable(GL_BLEND);

    //! Render UI
    if (uiPanel) {
        auto uiShader{AssetManager::GetShader("gui")};
        uiShader->Use();
        auto missingTex{AssetManager::GetTexture("default")};
        Sprite tempSprite{missingTex};
        missingTex->Use();
        spriteVAO->Use();
        for (auto& widget : uiPanel->widgets) {
            widget->UpdateTransform();
            uiShader->SetMatrix4("model", widget->GetModel());
            uiShader->SetVec2("spriteMinUV", tempSprite.GetMinUV());
            uiShader->SetVec2("spriteMaxUV", tempSprite.GetMaxUV());
            uiShader->SetVec4("color", glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
            uiShader->SetBool("flip", false);
            uiShader->SetMatrix4("uiProj", glm::ortho(0.f,
                                                      widget->GetPanel()->rect.size.x,
                                                      widget->GetPanel()->rect.size.y,
                                                      0.f));
            // uiShader->SetMatrix4("uiProj", glm::ortho(-widget->GetPanel()->rect.size.x / 2,
            //                                           widget->GetPanel()->rect.size.x / 2,
            //                                           -widget->GetPanel()->rect.size.y / 2,
            //                                           widget->GetPanel()->rect.size.y / 2));
            spriteVAO->Draw();
        }
    }
    glDisable(GL_BLEND);
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