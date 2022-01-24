#pragma once

#include <cuiui/math/types.hpp>

struct SceneVertex {
    f32vec2 pos;
    f32vec3 col;
};
struct UiVertex {
    f32vec2 pos;
    f32vec3 col;
};

std::array scene_vertices{
    // clang-format off
    SceneVertex{.pos{ 0.0f, -0.5f}, .col{0.0f, 1.0f, 0.0f}},
    SceneVertex{.pos{-0.5f,  0.5f}, .col{1.0f, 0.0f, 0.0f}},
    SceneVertex{.pos{ 0.5f,  0.5f}, .col{0.0f, 0.0f, 1.0f}},
    // clang-format on
};
std::array ui_vertices{
    // clang-format off
    UiVertex{.pos{ 1.0f, -0.9f}, .col{0.2f, 0.2f, 0.2f}},
    UiVertex{.pos{ 1.0f, -1.0f}, .col{0.2f, 0.2f, 0.2f}},
    UiVertex{.pos{-1.0f, -1.0f}, .col{0.2f, 0.2f, 0.2f}},
    UiVertex{.pos{ 1.0f, -0.9f}, .col{0.2f, 0.2f, 0.2f}},
    UiVertex{.pos{-1.0f, -1.0f}, .col{0.2f, 0.2f, 0.2f}},
    UiVertex{.pos{-1.0f, -0.9f}, .col{0.2f, 0.2f, 0.2f}},
    UiVertex{.pos{ 1.0f, -0.9f}, .col{0.3f, 0.3f, 0.3f}},
    UiVertex{.pos{ 1.0f, -1.0f}, .col{0.3f, 0.3f, 0.3f}},
    UiVertex{.pos{ 0.9f, -1.0f}, .col{0.3f, 0.3f, 0.3f}},
    UiVertex{.pos{ 1.0f, -0.9f}, .col{0.3f, 0.3f, 0.3f}},
    UiVertex{.pos{ 0.9f, -1.0f}, .col{0.3f, 0.3f, 0.3f}},
    UiVertex{.pos{ 0.9f, -0.9f}, .col{0.3f, 0.3f, 0.3f}},
    // clang-format on
};

const char *const scene_vert_src = R"(
#version 460 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec3 a_col;
layout(location = 0) out vec3 v_col;
layout(set = 0, binding = 0) uniform Ubo {
    mat4 view;
    mat4 ui_view;
} u;
void main() {
    gl_Position = u.ui_view * vec4(a_pos, 0, 1);
    v_col = a_col;
})";

const char *const scene_frag_src = R"(
#version 460 core
layout(location = 0) in vec3 v_col;
layout(location = 0) out vec4 o_col;
void main() {
    o_col = vec4(v_col, 1);
})";

const char *const ui_vert_src = R"(
#version 460 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec3 a_col;
layout(location = 0) out vec3 v_col;
layout(set = 0, binding = 0) uniform Ubo {
    mat4 view;
} u;
void main() {
    gl_Position = u.view * vec4(a_pos, 0, 1);
    v_col = a_col;
})";

const char *const ui_frag_src = R"(
#version 460 core
layout(location = 0) in vec3 v_col;
layout(location = 0) out vec4 o_col;
void main() {
    o_col = vec4(v_col, 1);
})";

struct Uniforms {
    f32mat4 view;
    f32mat4 ui_view;
} uniforms;

struct AppState {
    float mouse_x, mouse_y;
    uint32_t size_x, size_y;
};

#if COEL_USE_WIN32
#include <cuiui/platform/win32.hpp>
using AppWindow = cuiui::platform::win32::WindowBorderless;
using UiContext = cuiui::platform::win32::Context<AppWindow>;
#elif COEL_USE_X11
#include <cuiui/platform/x11.hpp>
using AppWindow = cuiui::platform::x11::Window;
using UiContext = cuiui::platform::x11::Context<AppWindow>;
#elif COEL_USE_NULL
#include <cuiui/platform/null.hpp>
using AppWindow = cuiui::platform::null::Window;
using UiContext = cuiui::platform::null::Context<AppWindow>;
#endif

using WindowHandle = cuiui::WindowHandle<AppWindow>;
