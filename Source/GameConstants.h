#pragma once

namespace GameConstants
{
    // Window settings
    constexpr int WINDOW_WIDTH = 854;
    constexpr int WINDOW_HEIGHT = 480;
    constexpr int FPS = 60;

    // Level settings
    constexpr int LEVEL_WIDTH = 11;
    constexpr int LEVEL_HEIGHT = 11;
    constexpr int TILE_SIZE = 32;
    constexpr int SPAWN_DISTANCE = 700;

    // Physics constants
    constexpr float MAX_SPEED_X = 750.0f;
    constexpr float MAX_SPEED_Y = 750.0f;
    constexpr float GRAVITY = 2000.0f;

    // Input constants
    constexpr int JOYSTICK_DEAD_ZONE = 8000;
}