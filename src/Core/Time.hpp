#pragma once

#include <stdint.h>

class Time {
public:
    // Real time in milliseconds since the game started
    static uint32_t GetMilisecondsSinceStartup();
    // Real time in seconds since the game started
    static float GetSecondsSinceStartup();
    
private:
    static void BeginFrame();

public:
    // Time in seconds at the start of the current frame
    static const float& time;
    // Time in milliseconds at the start of the current frame
    static const float& ticksCount;
    // Time in seconds between this and the previous frame
    static const float& deltaTime;
    // Time scale-independent in seconds between this and the previous frame
    static const float& unscaledDeltaTime;
    // Scale of the time (1.0f means realtime)
    static float timeScale;

private:
    static float _time;
    static float _ticksCount;
    static float _deltaTime;
    static float _unscaledDeltaTime; 

    friend class Engine;
};