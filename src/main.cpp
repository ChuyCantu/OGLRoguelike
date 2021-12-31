#include "Core/Engine.hpp"
#include "Core/Log.hpp"

int main(int argc, char** argv) {
    Log::Init();

    Engine app {"OGLRoguelike", 800, 600};
    app.Run();

    return 0;
}