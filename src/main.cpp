#include "Core/AssetsManager.hpp"
#include "Core/Engine.hpp"
#include "Core/Log.hpp"
#include "Utils/Random.hpp"


int main(int argc, char** argv) {
    Log::Init("SHDW", "%^[%d-%m-%Y %H:%M:%S] [%l]: %v%$");
    Random::SetSeed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    Engine app {"OGLRoguelike", 800, 600};
    app.Run();

    AssetsManager::Clear();

    return 0;
}