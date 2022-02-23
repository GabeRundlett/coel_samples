#pragma once

#include <cuiui/math/types.hpp>
#include <variant>

namespace cuiui {
    struct KeyEvent {
        u32 key;
        u32 action;
    };
    struct MouseButtonEvent {
        u32 key;
        u32 action;
    };
    struct MouseScrollEvent {
        f32vec2 offset;
    };
    struct MouseMotionEvent {
        f32vec2 pos;
    };
    namespace EventType {
        enum Id : size_t {
            KeyEvent,
            MouseButtonEvent,
            MouseMotionEvent,
            MouseScrollEvent,
        };
    }

    using Event = std::variant<KeyEvent, MouseButtonEvent, MouseMotionEvent, MouseScrollEvent>;
} // namespace cuiui
