#pragma once

namespace GameConstants
{
    // Window settings
    constexpr int WINDOW_WIDTH = 1280;
    constexpr int WINDOW_HEIGHT = 720;
    constexpr int FPS = 60;

    // Level settings
    constexpr int LEVEL_WIDTH = 21;
    constexpr int LEVEL_HEIGHT = 13;
    constexpr int TILE_SIZE = 64;
    constexpr int SPAWN_DISTANCE = 700;

    // Physics constants
    constexpr float MAX_SPEED_X = 750.0f;
    constexpr float MAX_SPEED_Y = 750.0f;
    constexpr float GRAVITY = 0.0f;

    // Input constants
    constexpr int JOYSTICK_DEAD_ZONE = 8000;
}