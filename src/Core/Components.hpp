#ifndef __COMPONENTS_H__
#define __COMPONENTS_H__

#include "AssetManager.hpp"
#include "Common.hpp"
#include "Event.hpp"
#include "Log.hpp"
#include "Rendering/VertexArray.hpp"
#include "Utils/MathExtras.hpp"

// #include <entt/entity/registry.hpp>

class GameObject;
class Sprite;
class Texture;
// class VertexArray;

// TODO: Use constructors to simplify work with the AddComponent function

struct Component {
    GameObject* gameobject;

    // bool enabled;
};

struct Transform : public Component {
    void SetPosition(const glm::vec3& position);
    void SetPosition(const glm::vec2& position);
    void SetRotation(const glm::quat& rotation);
    void SetScale(const glm::vec3& scale);
    void SetScale(const glm::vec2& scale);

    void UpdateTransform();

    const glm::vec3& GetPosition() const { return position; }
    const glm::quat& GetRotation() const { return rotation; }
    const glm::vec3& GetScale()    const { return scale;    }    
    const glm::mat4& GetModel()    const { return model;    }

private:
    glm::vec3 position {vec3::zero};
    glm::quat rotation {quaternion::identity};
    glm::vec3 scale    {1.0f}; // For objects whose pivot is not in the center (like Sprite), negative values will change the position relative to that pivot, consider flip instead
    glm::mat4 model    {1.0f};
    bool isDirty       {true};
};

struct SpriteRenderer : public Component {
    Ref<Sprite> sprite {MakeRef<Sprite>(AssetManager::GetTexture("default"))};
    glm::vec4 color    {glm::vec4{1.0f}};
    int renderOrder    {0};
    bool flip          {false};
};

struct Animator : public Component { // For this game, something this simple will do the job
    struct Frame
    {
        Ref<Texture> texture;
        float duration;
    };

    float timer           {0.0f};
    uint32_t currentFrame {0};
    std::vector<Frame> frames;
};

// Collider vs Collider will check if the location to move the object is occupied, if it is, there is a collision and may or not occupy the same space
// Collider vs TilemapCollider will check if the tile is different to 0 at the destination position, if it is, then there is a collision
struct Collider : public Component {
    bool isSolid       {true};
    bool ignoreSolid   {false};
};

struct TilemapCollider : public Component {
    bool isSolid {true};
};

struct Tile {
    int cost;
};

// This class is intended to be used when tiles with more information than the texture id is needed
template<class TileT>
struct Tilemap : Component {
    Tilemap(GameObject* gameobject, glm::ivec2 size) 
        : tiles{std::vector<TileT>(size.x * size.y)}, size{size}  {
        this->gameobject = gameobject;
    }

    TileT& GetTile(int x, int y) {
        uint32_t idx{x + y * (uint32_t)size.x};
        ASSERT(idx > tiles.size() - 1, "Index out of bounds. Values was: {} ({}, {}). Range was: [{} - {}]", idx, x, y, 0, tiles.size() - 1);
        return tiles[idx];
    }

    bool TryGetTile(int x, int y, TileT* outTile) {
        uint32_t idx{x + y * (uint32_t)size.x};
        if (idx < tiles.size()) {
            outTile = tiles[idx];
            return true;    
        }
        outTile = nullptr;
        return false;
    }

private:
    std::vector<TileT> tiles;
    glm::ivec2 size;
};

using tile_t = uint16_t;

struct TilemapRenderer : public Component {
private:
    glm::ivec2 size{0, 0};
    int tileSize{0};
    std::vector<tile_t> tiles;
    Ref<Texture> textureAtlas{AssetManager::GetTexture("default")};
    glm::ivec2 atlasTexSize{0, 0};
    int layer{0};
    Owned<VertexArray> mesh;

    bool isConstructed      {false};
    uint32_t uploadStartIdx {0};  // inclusive
    uint32_t uploadEndIdx   {0};  // exclusive
    int tilesTypeSize       {0};

public:
    TilemapRenderer(GameObject* gameobject, glm::ivec2 size, int tileSize, Ref<Texture> textureAtlas, int layer = 0);
    // This must be called in order to properly configure tilemap info
    void Construct(glm::ivec2 size, int tileSize, Ref<Texture> textureAtlas, int layer = 0);

    // TODO: Support autotiling
    void SetTile(int x, int y, tile_t tileIdx);
    tile_t GetTile(int x, int y);

    const glm::ivec2& GetSize() const { return size; }
    int GetTileSize() const { return tileSize; }
    Ref<Texture> GetTextureAtlas() const { return textureAtlas; }
    const glm::ivec2& GetAtlasTexSize() const { return atlasTexSize; }
    int GetLayer() const { return layer; }
    VertexArray* GetMesh() const { return mesh.get(); }
    bool IsConstructed() const { return isConstructed; }

    // TODO: Calculate other parameters
    void SetTextureAtlas(Ref<Texture> texture) { textureAtlas = texture; }

    void UpdateBufferData();
};

// void OnTilemapAdded(entt::registry& reg, entt::entity entity);

//+ Move component for Turn Based system
struct MoveComponent : public Component {
    void Move(glm::vec3 destination, float duration);
    void Update();
    void Cancel();
    void Teleport(glm::vec3 destination);

    const glm::vec3& GetSrcPosition() const { return srcPosition; }
    const glm::vec3& GetDestPosition() const { return destPosition; };
    const bool IsMoving() const { return srcPosition != destPosition; }

    Event<void()> onDestinationReached;
    Event<void()> onCancelation;

private:
    glm::vec3 srcPosition  {vec3::zero};
    glm::vec3 destPosition {vec3::zero};
    float timer            {0.0f};
    float time             {0.0f};
    
    bool startedMove       {false};

    friend class Scene;
};

#endif // __COMPONENTS_H__