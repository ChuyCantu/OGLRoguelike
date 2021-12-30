#include "Core/Log.hpp"

int main(int argc, char** argv) {
    Log::Init();

    LOG_TRACE("Log test.");

    return 0;
}