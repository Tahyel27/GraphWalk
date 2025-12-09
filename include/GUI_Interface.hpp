#pragma once

#include "raylib.h"

struct KeyEvent
{
    KeyboardKey key;
    enum class Type
    {
        PRESSED,
        RELEASED
    };
    Type type;
};

struct MouseEvent
{
    Vector2 pos;
    MouseButton button;
    enum class Type
    {
        PRESSED,
        RELEASED,
        MOVED
    };
    Type type;
};