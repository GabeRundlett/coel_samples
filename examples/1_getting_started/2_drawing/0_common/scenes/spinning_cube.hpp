#pragma once

#include "../render.hpp"

struct SpinningCubeScene {
    Shader shader = Shader(
        // clang-format off
        R"glsl(
            #version 460 core
            layout(location = 0) in vec2 a_pos;
            layout(location = 1) in vec3 a_col;
            layout(location = 0) out vec3 v_col;
            uniform mat4 modl_mat;
            void main() {
                v_col = a_col;
                gl_Position = modl_mat * vec4(a_pos, 0, 1);
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

    Uniform shader_modl_mat = shader.uniform("modl_mat");

    StaticMesh mesh = StaticMesh({
        {.size = 2, .type = GL_FLOAT},
        {.size = 3, .type = GL_FLOAT},
    });

    SpinningCubeScene() {
        mesh.infos[0].set_data(std::array{
            // clang-format off
            -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // bottom left
            -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, // top left
             0.5f, -0.5f,   0.0f, 0.0f, 1.0f, // bottom right
             0.5f,  0.5f,   1.0f, 0.0f, 0.0f, // top right
             
            -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // bottom left
            -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, // top left
             0.5f, -0.5f,   0.0f, 0.0f, 1.0f, // bottom right
             0.5f,  0.5f,   1.0f, 0.0f, 0.0f, // top right
             
            -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // bottom left
            -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, // top left
             0.5f, -0.5f,   0.0f, 0.0f, 1.0f, // bottom right
             0.5f,  0.5f,   1.0f, 0.0f, 0.0f, // top right
             
            -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // bottom left
            -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, // top left
             0.5f, -0.5f,   0.0f, 0.0f, 1.0f, // bottom right
             0.5f,  0.5f,   1.0f, 0.0f, 0.0f, // top right
             
            -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // bottom left
            -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, // top left
             0.5f, -0.5f,   0.0f, 0.0f, 1.0f, // bottom right
             0.5f,  0.5f,   1.0f, 0.0f, 0.0f, // top right
             
            -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // bottom left
            -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, // top left
             0.5f, -0.5f,   0.0f, 0.0f, 1.0f, // bottom right
             0.5f,  0.5f,   1.0f, 0.0f, 0.0f, // top right
            // clang-format on
        });

        mesh.infos[0].use_ibo(std::array{
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
        mesh.infos[0].draw();
    }
};
