#pragma once

#include <array>

const char *const vert_src = R"(
#version 460 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_tex;
layout(location = 0) out vec2 v_tex;
void main() {
    v_tex = a_tex;
    gl_Position = vec4(a_pos, 0, 1);
})";

const char *const frag_src = R"(
#version 460 core
layout(location = 0) in vec2 v_tex;
layout(location = 0) out vec4 o_col;
layout(binding = 0) uniform sampler2D tex;
void main() {
    o_col = texture(tex, v_tex);
    o_col.w = 1;
    // o_col = vec4(v_tex, 0, 1);
})";

std::array<float, 4 * 4> vertices{
    // clang-format off
    -0.5f, -0.5f,   0.0f, 0.0f, // bottom left
    -0.5f,  0.5f,   0.0f, 1.0f, // top    left
     0.5f, -0.5f,   1.0f, 0.0f, // bottom right
     0.5f,  0.5f,   1.0f, 1.0f, // top    right
    // clang-format on
};

std::array<uint32_t, 6> indices{
    // clang-format off
    0, 1, 2,
    1, 3, 2,
    // clang-format on
};
