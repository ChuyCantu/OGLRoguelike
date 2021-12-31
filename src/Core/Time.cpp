#include "Time.hpp"

#include <SDL.h>

const float& Time::time              {_time};
const float& Time::ticksCount        {_ticksCount};
const float& Time::deltaTime         {_deltaTime};
const float& Time::unscaledDeltaTime {_unscaledDeltaTime};
float Time::timeScale                {1.0f};

float Time::_time                    {0.0f};
float Time::_ticksCount              {0.0f};
float Time::_deltaTime               {0.0f};
float Time::_unscaledDeltaTime       {0.0f};

uint32_t Time::GetMilisecondsSinceStartup() {
    return SDL_GetTicks();
}

float Time::GetSecondsSinceStartup() {
    return SDL_GetTicks() / 1000.f;
}

void Time::BeginFrame() {
    // Limit frame rate (60 fps)
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), _ticksCount + 16));

    _unscaledDeltaTime = (SDL_GetTicks() - _ticksCount) / 1000.f;
    _deltaTime = _unscaledDeltaTime * timeScale;
    _ticksCount = SDL_GetTicks();
    _time = _ticksCount / 1000.f;

    // // Clamp max deltaTime value 
    // if (_deltaTime >= 0.05f)
    //     _deltaTime = 0.05f;
}
