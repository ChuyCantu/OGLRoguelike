#ifndef __TILEMAP_H__
#define __TILEMAP_H__

#include "Core/GameObject.hpp"

class Tilemap : public GameObject {
public:
    Tilemap(Scene* scene);
    ~Tilemap() override;

    void Start() override;
    void Update() override;
    void OnEnable() override;
    void OnDisable() override;
    void OnDestroy() override;
};

#endif // __TILEMAP_H__