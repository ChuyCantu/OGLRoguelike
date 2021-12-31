#include "Core/Engine.hpp"
#include "Core/Log.hpp"

int main(int argc, char** argv) {
    Log::Init("SHDW", "%^[%d-%m-%Y %H:%M:%S] [%l] %v%$");

    Engine app {"OGLRoguelike", 800, 600};
    app.Run();

    return 0;
}