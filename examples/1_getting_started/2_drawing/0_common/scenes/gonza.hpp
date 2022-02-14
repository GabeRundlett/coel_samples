#pragma once

#include <1_getting_started/2_drawing/0_common/render.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct GonzaScene {
    Shader shader = Shader(
        // clang-format off
        R"glsl(
            #version 460 core
            layout(location = 0) in vec3 a_pos;
            layout(location = 1) in vec3 a_col;
            layout(location = 2) in vec2 a_tex;
            layout(location = 0) out vec3 v_col;
            uniform mat4 proj_mat;
            uniform mat4 view_mat;
            uniform mat4 modl_mat;
            void main() {
                vec4 world_pos = modl_mat * vec4(a_pos, 1);
                v_col = a_col;
                gl_Position = proj_mat * view_mat * world_pos;
            }
        )glsl",
        R"glsl(
            #version 460 core
            layout(location = 0) in vec3 v_col;
            layout(location = 0) out vec4 o_col;
            void main() {
                o_col = vec4(v_col, 1);
            }
        )glsl"
        // clang-format on
    );

    Uniform shader_proj_mat = shader.uniform("proj_mat");
    Uniform shader_view_mat = shader.uniform("view_mat");
    Uniform shader_modl_mat = shader.uniform("modl_mat");

    StaticMesh mesh = StaticMesh("examples/0_assets/gonza/export/y-up/gonza.gltf");

    using clock = std::chrono::high_resolution_clock;
    clock::time_point start;

    f32 aspect = 1.0f;

    GonzaScene() {
        start = clock::now();
    }

    void draw() {
        shader.use();

        // auto now = clock::now();
        // auto start_diff = now - start;
        // auto elapsed = std::chrono::duration<float>(start_diff).count();

        auto proj_mat = (perspective(radians(90.0f), aspect, 0.01f, 100.0f));

        f32vec3 rot = {0.0f, 1.0f * radians(90.0f - 65.0f), 0.0f};
        f32vec3 pos = {-1.5f, -1.5f, -1.5f};

        auto view_mat = rotate(rotate(f32mat4::identity(), rot.y, {1, 0, 0}), rot.x, {0, 1, 0}) * translate(f32mat4::identity(), pos);

        shader_proj_mat.send_mat4(proj_mat);
        shader_view_mat.send_mat4(view_mat);
        mesh.draw([&](const auto &mesh_info) {
            shader_modl_mat.send_mat4(mesh_info.modl_mat);
        });
    }
};
