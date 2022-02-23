#pragma once

#include "../render.hpp"
#include <cuiui/math/utility.hpp>

struct SpinningCubeScene {
    Shader shader = Shader(
        // clang-format off
        R"glsl(
            #version 460 core
            layout(location = 0) in vec3 a_pos;
            layout(location = 1) in vec3 a_col;
            layout(location = 0) out vec3 v_col;
            uniform mat4 proj_mat;
            uniform mat4 view_mat;
            uniform mat4 modl_mat;
            void main() {
                v_col = a_col;
                gl_Position = proj_mat * view_mat * modl_mat * vec4(a_pos, 1);
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

    StaticMesh mesh = StaticMesh({
        {.size = 3, .type = GL_FLOAT},
        {.size = 3, .type = GL_FLOAT},
    });

    using clock = std::chrono::high_resolution_clock;
    clock::time_point start;

    f32 aspect = 1.0f;

    SpinningCubeScene() {
        mesh.set_data(std::array{
            // clang-format off
            -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f,
            // clang-format on
        });

        mesh.use_ibo(std::array{
            // clang-format off
            0 + 0 * 4, 1 + 0 * 4, 2 + 0 * 4,
            1 + 0 * 4, 3 + 0 * 4, 2 + 0 * 4,
            0 + 1 * 4, 1 + 1 * 4, 2 + 1 * 4,
            1 + 1 * 4, 3 + 1 * 4, 2 + 1 * 4,
            0 + 2 * 4, 1 + 2 * 4, 2 + 2 * 4,
            1 + 2 * 4, 3 + 2 * 4, 2 + 2 * 4,
            0 + 3 * 4, 1 + 3 * 4, 2 + 3 * 4,
            1 + 3 * 4, 3 + 3 * 4, 2 + 3 * 4,
            0 + 4 * 4, 1 + 4 * 4, 2 + 4 * 4,
            1 + 4 * 4, 3 + 4 * 4, 2 + 4 * 4,
            0 + 5 * 4, 1 + 5 * 4, 2 + 5 * 4,
            1 + 5 * 4, 3 + 5 * 4, 2 + 5 * 4,
            // clang-format on
        });
    }

    void draw() {
        shader.use();

        auto now = clock::now();
        auto start_diff = now - start;
        auto elapsed = std::chrono::duration<float>(start_diff).count();

        auto proj_mat = (perspective(radians(90.0f), aspect, 0.01f, 100.0f));
        auto view_mat = translate(f32mat4::identity(), {0, 0, -1.5f});
        auto modl_mat = rotate(rotate(f32mat4::identity(), elapsed * 1.43f, {0, 1, 0}), elapsed * 0.41f, {1, 0, 0});

        shader_proj_mat.send_mat4(proj_mat);
        shader_view_mat.send_mat4(view_mat);
        shader_modl_mat.send_mat4(modl_mat);
        mesh.draw();
    }
};
