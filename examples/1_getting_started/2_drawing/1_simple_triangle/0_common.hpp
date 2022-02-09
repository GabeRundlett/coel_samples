#pragma once

#include <array>

const char *const vert_src = R"(
#version 460 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec3 a_col;
layout(location = 0) out vec3 v_col;
void main() {
    v_col = a_col;
    gl_Position = vec4(a_pos, 0, 1);
})";

const char *const frag_src = R"(
#version 460 core
layout(location = 0) in vec3 v_col;
layout(location = 0) out vec4 o_col;
void main() {
    o_col = vec4(v_col, 1);
})";

std::array<float, 3 * 5> vertices{
    // clang-format off
    -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // bottom left
    -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, // top left
     0.5f, -0.5f,   0.0f, 0.0f, 1.0f, // bottom right
    // clang-format on
};
