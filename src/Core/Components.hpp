#ifndef __COMPONENTS_H__
#define __COMPONENTS_H__

#include "AssetManager.hpp"
#include "Common.hpp"
#include "Rendering/VertexArray.hpp"
#include "Utils/MathExtras.hpp"

class GameObject;
class Sprite;
class Texture;
// class VertexArray;

struct Component {
    GameObject* gameobject;
};

struct Transform : public Component {
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::quat& rotation);
    void SetScale(const glm::vec3& scale);

    void UpdateTransform();

    const glm::vec3& GetPosition() const { return position; }
    const glm::quat& GetRotation() const { return rotation; }
    const glm::vec3& GetScale()    const { return scale;    }    
    const glm::mat4& GetModel()    const { return model;    }

private:
    glm::vec3 position {vec3::zero};
    glm::quat rotation {quaternion::identity};
    glm::vec3 scale    {1.0f};
    glm::mat4 model    {1.0f};
    bool isDirty       {true};
};

struct SpriteRenderer : public Component {
    Ref<Sprite> sprite {MakeRef<Sprite>(AssetManager::GetTexture("default"))};
    glm::vec4 color    {glm::vec4{1.0f}};
    int renderOrder    {0};
};


using tile_t = uint16_t;

struct TilemapRenderer : public Component {
private:
    glm::ivec2 size              {0, 0};
    int tileSize                 {0};
    std::vector<tile_t> tiles;
    Ref<Texture> textureAtlas    {AssetManager::GetTexture("default")};
    glm::ivec2 atlasTexSize      {0, 0};
    int layer                    {0};
    Owned<VertexArray> mesh;   

    bool isConstructed           {false};
    uint32_t uploadStartIdx      {0}; // inclusive
    uint32_t uploadEndIdx        {0}; // exclusive
    int tilesTypeSize            {0};

public:
    // This must be called in order to properly configure tilemap info
    void Construct(glm::ivec2 size, int tileSize, Ref<Texture> textureAtlas, int layer = 0);

    void SetTile(int x, int y, tile_t tileIdx);

    const glm::ivec2& GetSize()          const { return size; }
    int GetTileSize()                    const { return tileSize; }
    Ref<Texture> GetTextureAtlas()       const { return textureAtlas; }   
    const glm::ivec2& GetAtlasTexSize()  const { return atlasTexSize; }     
    int GetLayer()                       const { return layer; }                   
    VertexArray* GetMesh();
    bool IsConstructed()                 const { return isConstructed; }

    void SetTextureAtlas(Ref<Texture> texture) { textureAtlas = texture; }

    void UpdateBufferData();
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

#endif // __COMPONENTS_H__