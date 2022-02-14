#pragma once

#include <array>
#include <iostream>

#include <glad/glad.h>

#include "model.hpp"

struct RenderPass {
    u32vec2 size;
    void begin() {
        glViewport(0, 0, size.x, size.y);
        glClearColor(0.6f, 0.7f, 1.0f, 1.0f);
        glClearDepth(1.0f);
        glDepthFunc(GL_LEQUAL);
        // glClearDepth(0.0f);
        // glDepthFunc(GL_GREATER);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
    }
};

struct BlitWindowPass : RenderPass {
    void begin(auto &window) {
        size = window->size;
        RenderPass::begin();
    }
};

struct Uniform {
    int32_t location;

    // void send_mat4(const f32mat4 &m) {
    //     glUniformMatrix4fv(location, 1, false, reinterpret_cast<const GLfloat *>(&m));
    // }
    void send_mat4(const auto &m) {
        glUniformMatrix4fv(location, 1, false, reinterpret_cast<const GLfloat *>(&m));
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

    Uniform uniform(const char *const name) {
        return {.location = glGetUniformLocation(shader_program_id, name)};
    }
};

struct AttribDesc {
    u32 size, type;
};

struct StaticMesh {
    struct MeshInfo {
        uint32_t vao_id;
        uint32_t vbo_id;
        uint32_t ibo_id;

        size_t vertex_size = 0;
        size_t vertices_n = 0;
        size_t indices_n = 0;

        f32mat4 modl_mat;

        MeshInfo(const std::initializer_list<AttribDesc> &attribs) {
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
            vertex_size = offset;
        }

        MeshInfo(const MeshInfo &) = delete;
        MeshInfo &operator=(const MeshInfo &) = delete;

        MeshInfo(MeshInfo &&other) {
            *this = std::move(other);
        }
        MeshInfo &operator=(MeshInfo &&other) {
            vao_id = other.vao_id;
            vbo_id = other.vbo_id;
            ibo_id = other.ibo_id;
            vertex_size = other.vertex_size;
            vertices_n = other.vertices_n;
            indices_n = other.indices_n;
            modl_mat = other.modl_mat;

            other.vao_id = std::numeric_limits<uint32_t>::max();
            other.vbo_id = std::numeric_limits<uint32_t>::max();
            other.ibo_id = std::numeric_limits<uint32_t>::max();

            return *this;
        }

        ~MeshInfo() {
            glDeleteBuffers(1, &ibo_id);
            glDeleteBuffers(1, &vbo_id);
            glDeleteVertexArrays(1, &vao_id);
        }

        void set_index_data(const void *data, size_t size) {
            indices_n = size / sizeof(uint32_t);
            glNamedBufferData(ibo_id, size, data, GL_STATIC_DRAW);
        }
        void set_index_data(const auto &list) {
            set_index_data(list.data(), list.size() * sizeof(list[0]));
        }
        void use_ibo(auto &&...args) {
            glCreateBuffers(1, &ibo_id);
            glVertexArrayElementBuffer(vao_id, ibo_id);

            set_index_data(args...);
        }

        void set_data(const void *data, size_t size) {
            vertices_n = size / vertex_size;
            glNamedBufferData(vbo_id, size, data, GL_STATIC_DRAW);
        }

        void set_data(const auto &list) {
            set_data(list.data(), list.size() * sizeof(list[0]));
        }

        void draw() const {
            glBindVertexArray(vao_id);
            if (indices_n == 0) {
                glDrawArrays(GL_TRIANGLES, 0, vertices_n);
            } else {
                glDrawElements(GL_TRIANGLES, indices_n, GL_UNSIGNED_INT, nullptr);
            }
        }
    };
    std::vector<MeshInfo> infos;

    StaticMesh(const std::initializer_list<AttribDesc> &attribs) {
        infos.push_back(attribs);
    }

    StaticMesh(const std::filesystem::path &path) {
        auto assimp_model = AssimpModel(path);
        for (auto &node : assimp_model.source->nodes) {
            infos.push_back({
                {.size = 3, .type = GL_FLOAT},
                {.size = 3, .type = GL_FLOAT},
                {.size = 2, .type = GL_FLOAT},
            });
            auto &info = infos.back();
            info.set_data(node.vertices);
            info.use_ibo(node.indices);
            info.modl_mat = node.modl_mat;
        }
    }

    void draw(const auto &fn) {
        for (const auto &info : infos) {
            fn(info);
            info.draw();
        }
    }
};

struct RenderContext {
    coel::opengl::Context gl_ctx;

    void attach_to(auto &window) {
        gl_ctx.attach(window);
        gl_ctx.make_current();
        gladLoadGL();
    }

    void flush() {
        gl_ctx.swap_buffers();
    }
};
