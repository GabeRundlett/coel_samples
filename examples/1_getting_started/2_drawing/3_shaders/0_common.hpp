#pragma once

#include <array>

#include <glad/glad.h>
#include <iostream>

struct RenderPass {
    u32vec2 size;
    void begin() {
        glViewport(0, 0, size.x, size.y);
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
};

struct BlitWindowPass : RenderPass {
    void begin(auto &window) {
        size = window->size;
        RenderPass::begin();
    }
};

struct Shader {
    uint32_t shader_program_id;

    Shader(const char *const vert_src, const char *const frag_src) {
        auto attach_shader = [](auto program_id, auto shader_type, auto shader_code) {
            auto shader_id = glCreateShader(shader_type);
            glShaderSource(shader_id, 1, &shader_code, nullptr);
            glCompileShader(shader_id);
            int32_t param;
            glGetShaderiv(shader_id, GL_COMPILE_STATUS, &param);
            if (param == GL_FALSE) {
                param = 0;
                glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &param);
                std::vector<char> info_log(static_cast<size_t>(param) + 100);
                glGetShaderInfoLog(shader_id, param, &param, &info_log[0]);
                glDeleteShader(shader_id);
                std::cout << info_log.data() << '\n';
            }
            glAttachShader(program_id, shader_id);
            return shader_id;
        };
        shader_program_id = glCreateProgram();
        auto vert_shader_id = attach_shader(shader_program_id, GL_VERTEX_SHADER, vert_src);
        auto frag_shader_id = attach_shader(shader_program_id, GL_FRAGMENT_SHADER, frag_src);
        glLinkProgram(shader_program_id);
        glDetachShader(shader_program_id, vert_shader_id);
        glDetachShader(shader_program_id, frag_shader_id);
        glDeleteShader(vert_shader_id);
        glDeleteShader(frag_shader_id);
    }

    ~Shader() {
        glDeleteProgram(shader_program_id);
    }

    void use() {
        glUseProgram(shader_program_id);
    }
};

struct AttribDesc {
    u32 size, type;
};

struct StaticMesh {
    uint32_t vao_id;
    uint32_t vbo_id;

    StaticMesh(const std::initializer_list<AttribDesc> &attribs) {
        glCreateVertexArrays(1, &vao_id);
        glCreateBuffers(1, &vbo_id);

        auto gl_attrib_type_size = [](u32 type) -> size_t {
            switch (type) {
            case GL_FLOAT: return sizeof(float);
            default: return 1;
            }
        };

        u32 index = 0;
        size_t offset = 0;
        for (const auto &attrib : attribs) {
            glEnableVertexArrayAttrib(vao_id, index);
            glVertexArrayAttribBinding(vao_id, index, 0);
            glVertexArrayAttribFormat(vao_id, index, attrib.size, attrib.type, GL_FALSE, offset);
            offset += gl_attrib_type_size(attrib.type) * attrib.size;
            index++;
        }
        glVertexArrayVertexBuffer(vao_id, 0, vbo_id, 0, offset);
    }

    ~StaticMesh() {
        glDeleteBuffers(1, &vbo_id);
        glDeleteVertexArrays(1, &vao_id);
    }

    void set_data(const void *data, size_t size) {
        glNamedBufferData(vbo_id, size, data, GL_STATIC_DRAW);
    }

    void set_data(const auto &list) {
        set_data(list.data(), list.size() * sizeof(list[0]));
    }

    void draw(size_t n) {
        glBindVertexArray(vao_id);
        glDrawArrays(GL_TRIANGLES, 0, n);
    }
};

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

    StaticMesh tri = StaticMesh({
        {.size = 2, .type = GL_FLOAT},
        {.size = 3, .type = GL_FLOAT},
    });

    TriangleScene() {
        tri.set_data(std::array{
            // clang-format off
            -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // bottom left
            -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, // top left
             0.5f, -0.5f,   0.0f, 0.0f, 1.0f, // bottom right
            // clang-format on
        });
    }

    void draw() {
        shader.use();
        tri.draw(3);
    }
};

struct GonzaScene {
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

    StaticMesh tri = StaticMesh("C:/Users/gabe/Downloads/Gonza/export/y-up/gonza.gltf");

    GonzaScene() {
        tri.set_data(std::array{
            // clang-format off
            -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // bottom left
            -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, // top left
             0.5f, -0.5f,   0.0f, 0.0f, 1.0f, // bottom right
            // clang-format on
        });
    }

    void draw() {
        shader.use();
        tri.draw(3);
    }
};
