#pragma once

#include "../render.hpp"

struct TriangleScene {
    Shader shader = Shader(
        // clang-format off
        R"glsl(
            #version 460 core
            layout(location = 0) in vec2 a_pos;
            layout(location = 1) in vec3 a_col;
            layout(location = 0) out vec3 v_col;
            void main() {
                v_col = a_col;
                gl_Position = vec4(a_pos, 0, 1);
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

    StaticMesh mesh = StaticMesh({
        {.size = 2, .type = GL_FLOAT},
        {.size = 3, .type = GL_FLOAT},
    });

    TriangleScene() {
        mesh.infos[0].set_data(std::array{
            // clang-format off
            -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // bottom left
            -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, // top left
             0.5f, -0.5f,   0.0f, 0.0f, 1.0f, // bottom right
            // clang-format on
        });
    }

    void draw() {
        shader.use();
        mesh.infos[0].draw();
    }
};
