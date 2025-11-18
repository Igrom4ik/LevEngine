#pragma once
#include <cstdint>

namespace LEN
{
    enum class Key : uint16_t
    {
        Unknown = 0,

        // Letters
        A, B, C, /* ... */ Z,

        // Numbers
        Num0, Num1, /* ... */ Num9,

        // Function keys
        F1, F2, /* ... */ F12,

        // Arrows
        Left,
        Right,
        Up,
        Down,

        // Control keys
        Space,
        Escape,
        Enter,

        // Mouse
        MouseLeft,
        MouseRight,
        MouseMiddle,

        // ...
        Count
    };
}