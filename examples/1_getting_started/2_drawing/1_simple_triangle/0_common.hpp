#pragma once

#include <array>

const char *const glsl_vert_src = R"(
#version 460 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec3 a_col;
layout(location = 0) out vec3 v_col;
void main() {
    v_col = a_col;
    gl_Position = vec4(a_pos, 0, 1);
})";

const char *const glsl_frag_src = R"(
#version 460 core
layout(location = 0) in vec3 v_col;
layout(location = 0) out vec4 o_col;
void main() {
    o_col = vec4(v_col, 1);
})";

const char *const hlsl_vert_src = R"(
struct VertIn {
    float2 a_pos : POS; 
    float3 a_col : COL;
};
struct FragIn {
    float4 o_pos : SV_Position; 
    float3 v_col : COL;
};
FragIn main(VertIn input) {
    FragIn result;
    result.v_col = input.a_col;
    result.o_pos = float4(input.a_pos, 0, 1);
    return result;
})";

const char *const hlsl_frag_src = R"(
struct FragIn {
    float4 o_pos : SV_Position; 
    float3 v_col : COL;
};
float4 main(FragIn input) : SV_TARGET0 {
    return float4(input.v_col, 1);
})";

std::array<float, 3 * 5> vertices{
    // clang-format off
    -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // bottom left
    -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, // top left
     0.5f, -0.5f,   0.0f, 0.0f, 1.0f, // bottom right
    // clang-format on
};
