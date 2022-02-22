#ifndef __COMPONENTS_H__
#define __COMPONENTS_H__

#include "AssetManager.hpp"
#include "Common.hpp"
#include "Event.hpp"
#include "Log.hpp"
#include "Utils/MathExtras.hpp"
#include "Utils/Color.hpp"

#include <entt/entity/registry.hpp>

class GameObject;
class Sprite;
class Texture;

struct Component {
    GameObject* gameobject {nullptr};
    bool enabled {true};
};

struct Transform : public Component {
    Transform() { }

    void SetPosition(const glm::vec3& position);
    void SetPosition(const glm::vec2& position);
    void SetAbsolutePosition(const glm::vec3& position);
    void SetAbsolutePosition(const glm::vec2& position);
    void SetRotation(const glm::quat& rotation);
    void SetScale(const glm::vec3& scale);
    void SetScale(const glm::vec2& scale);

    void AddChild(GameObject* gameobject);
    void RemoveChild(GameObject* gameobject);
    void RemoveChildren();

    void UpdateChildrenPosition();
    void UpdateRelativePosition();
    void UpdateTransform();

    const glm::vec3& GetPosition()         const { return position; }
    const glm::vec3& GetAbsolutePosition() const { return absolutePosition; }
    const glm::quat& GetRotation()         const { return rotation; }
    const glm::vec3& GetScale()            const { return scale;    }    
    const glm::mat4& GetModel()            const { return model;    }
    GameObject* const GetParent()          const;
    entt::entity GetParentEntity()         const { return parent; }
    bool HasParent()                       const { return parent != entt::null; }
    const std::vector<entt::entity>& GetChildren() const { return children; }

private:
    glm::vec3 position         {vec3::zero};
    glm::vec3 absolutePosition {vec3::zero};
    glm::quat rotation         {quaternion::identity};
    glm::vec3 scale            {1.0f}; // For objects whose pivot is not in the center (like Sprite), negative values will change the position relative to that pivot, consider flip instead
    glm::mat4 model            {1.0f};
    bool isDirty               {true};

    entt::entity parent        {entt::null};
    std::vector<entt::entity> children;

    using Component::enabled;
};

struct SpriteRenderer : public Component {
    SpriteRenderer();
    SpriteRenderer(Ref<Sprite> sprite, Color color = ColorNames::white, int renderOrder = 0, const glm::vec2& pivot = glm::vec2{0.0f, 0.0f});

    Ref<Sprite> sprite; //{MakeRef<Sprite>(AssetManager::GetTexture("missing"))};
    Color color        {ColorNames::white};
    int renderOrder    {0};
    // (0, 0) is bottom-left corner
    glm::vec2 pivot    {0.0f, 0.0f};
};

struct Animator : public Component { // For this game, something this simple will do the job
    struct Frame
    {
        Ref<Texture> texture;
        float duration;
    };

    Animator() { }

    float timer           {0.0f};
    uint32_t currentFrame {0};
    std::vector<Frame> frames;
};

// Collider vs Collider will check if the location to move the object is occupied, if it is, there is a collision and may or not occupy the same space
// Collider vs TilemapCollider will check if the tile is different to 0 at the destination position, if it is, then there is a collision
struct Collider : public Component {
    Collider(bool isSolid = true, bool ignoreSolid = false);

    bool isSolid       {true};
    bool ignoreSolid   {false};
};

struct TilemapCollider : public Component {
    TilemapCollider(bool isSolid = true);

    bool isSolid {true};
};

//+ New Tilemap System =============================================================================

struct TileRule {

};

struct AutotileData {
    std::map<int, Ref<Sprite>> sprites;
};

struct AnimatedTileData {
    std::vector<Ref<Sprite>> sprites;
};

struct TileAnimator {
    const AnimatedTileData* data;
    size_t currentSprite {0};
};

struct RandomTileData {
    std::vector<Ref<Sprite>> sprites;
};

enum TileType { 
    Simple,   // Display a single sprite
    Animated, // Display a sprite animation given the sprite frames
    Random,   // Display a random sprite from the list given
    Autotile    
};

struct Tilemap;

class Tile {
public:
    ~Tile();

    void Start(const glm::ivec2& pos, Tilemap& tilemap);
    void Refresh(const glm::ivec2& pos);

    // Copies data from one Tile to another. This must be used before sending the tile into the tilemap since it may behave wrong if the copied tile is an autotile
    void CopyTo(Tile* other) const;

    Sprite* GetRenderSprite() const;

public:
    // Used for all types of tiles
    Ref<Sprite> defaultSprite {nullptr};
    
    TileType type {TileType::Simple};
    
    const RandomTileData* randomTileData     {nullptr};
    const AnimatedTileData* animatedTileData {nullptr};
    const AutotileData* autotileData         {nullptr};
    uint8_t neighbors {0};
    int layer {0}; // Used for autotiled tiles. If the layer on a neighbour tile is the same as the autotiled tile, it will be considered for the tile bitmask

private:
    Tilemap* tilemap {nullptr};
    entt::entity entity {entt::null}; // Used for fast iteration on renderer and animation components

    friend struct Tilemap;
};

class Chunk {
public:
    Chunk(int chunkSize, int tileSize);

    Tile* SetTile(int x, int y, Owned<Tile> tile);
    Tile* GetTile(int x, int y);

    void Draw(const glm::vec2& worldPos, Color color);

    int GetChunkSize() const { return chunkSize; }

private:
    // Size per side of the chunk (so the chunk tiles count would be chunkSize * chunkSize)
    int chunkSize; 
    int tileSize;
    std::vector<Owned<Tile>> tiles;
};

namespace std {
bool operator<(const glm::ivec2& lhs, const glm::ivec2& rhs);
}

struct Tilemap : Component {
    Tilemap(int tileSize = 16, Color color = ColorNames::white, int renderOrder = 0, int chunksSize = 32);

    Tile* SetTile(int x, int y, Owned<Tile> tile);
    Tile* GetTile(int x, int y);
    
    void Update();
    void RefreshVisibleChunks(const glm::ivec2& centerChunk);
    void Render();

    glm::ivec2 WorldSpace2TilemapSpace(int x, int y);
    glm::ivec2 TilemapSpace2WorldSpace(int x, int y);

public:
    int tileSize   {16};
    Color color    {ColorNames::white};
    // Chunks position is the position of the tile in the bottom-left corner of the chunk in the Tilemap space
    int renderOrder{0};
    int chunksSize {32};

    float animationsDuration {0};

private:
    std::map<glm::ivec2, Chunk> chunks;

    std::array<std::pair<glm::ivec2, Chunk*>, 9> visibleChunks;
    glm::ivec2 lastCameraPosition {0, 0};

    float animatorTimer {0};

    entt::registry tilesRegistry;

    friend class Tile;
};

//+ Move component for Turn Based system

struct MoveComponent : public Component {
    MoveComponent() { }

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