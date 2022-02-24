#pragma once

#include "model.hpp"

#include <array>
#include <iostream>

#include <glad/glad.h>
#include <coel/opengl/core.hpp>

struct RenderPass {
    u32vec2 size;
    void begin() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, size.x, size.y);
        glClearColor(0.6f, 0.7f, 1.0f, 1.0f);
        // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

    void send_int(int i) {
        glUniform1i(location, i);
    }
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

struct Texture {
    uint32_t id;

    Texture(const u8 *bitmap_data, size_t sx, size_t sy) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(sx), static_cast<GLsizei>(sy), 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    ~Texture() {
        glDeleteTextures(1, &id);
    }

    void bind_slot(u32 slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, id);
    }
};

struct AttribDesc {
    u32 size, type;
};

struct Mesh {
    uint32_t vao_id;
    uint32_t vbo_id;
    uint32_t ibo_id;

    size_t vertex_size = 0;
    size_t vertices_n = 0;
    size_t indices_n = 0;
    size_t max_vertices_n = 0;
    size_t max_indices_n = 0;

    f32mat4 modl_mat;

    Mesh(const std::initializer_list<AttribDesc> &attribs) {
        glCreateVertexArrays(1, &vao_id);
        glCreateBuffers(1, &vbo_id);
        auto gl_attrib_type_size = [](u32 type) -> size_t {
            switch (type) {
            case GL_FLOAT: return sizeof(float);
            default: return 1;
            }
        };
        u32 index = 0;
        u32 offset = 0;
        for (const auto &attrib : attribs) {
            glEnableVertexArrayAttrib(vao_id, index);
            glVertexArrayAttribBinding(vao_id, index, 0);
            glVertexArrayAttribFormat(vao_id, index, attrib.size, attrib.type, GL_FALSE, offset);
            offset += static_cast<u32>(gl_attrib_type_size(attrib.type) * attrib.size);
            index++;
        }
        glVertexArrayVertexBuffer(vao_id, 0, vbo_id, 0, offset);
        vertex_size = offset;
    }

    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    Mesh(Mesh &&other) {
        *this = std::move(other);
    }
    Mesh &operator=(Mesh &&other) {
        vao_id = other.vao_id;
        vbo_id = other.vbo_id;
        ibo_id = other.ibo_id;
        vertex_size = other.vertex_size;
        vertices_n = other.vertices_n;
        indices_n = other.indices_n;
        max_vertices_n = other.max_vertices_n;
        max_indices_n = other.max_indices_n;
        modl_mat = other.modl_mat;
        other.vao_id = std::numeric_limits<uint32_t>::max();
        other.vbo_id = std::numeric_limits<uint32_t>::max();
        other.ibo_id = std::numeric_limits<uint32_t>::max();
        return *this;
    }

    ~Mesh() {
        glDeleteBuffers(1, &ibo_id);
        glDeleteBuffers(1, &vbo_id);
        glDeleteVertexArrays(1, &vao_id);
    }

    void set_index_data(const void *data, size_t size, uint32_t usage) {
        indices_n = size / sizeof(uint32_t);
        max_indices_n = indices_n;
        glNamedBufferData(ibo_id, size, data, usage);
    }
    void set_index_data(const auto &list, uint32_t usage) {
        set_index_data(list.data(), list.size() * sizeof(list[0]), usage);
    }
    void use_ibo(auto &&...args) {
        glCreateBuffers(1, &ibo_id);
        glVertexArrayElementBuffer(vao_id, ibo_id);
        set_index_data(args...);
    }

    void set_data(const void *data, size_t size, uint32_t usage) {
        vertices_n = size / vertex_size;
        max_vertices_n = vertices_n;
        glNamedBufferData(vbo_id, size, data, usage);
    }

    void set_data(const auto &list, uint32_t usage) {
        set_data(list.data(), list.size() * sizeof(list[0]), usage);
    }

    void draw() const {
        glBindVertexArray(vao_id);
        if (max_indices_n == 0) {
            glDrawArrays(GL_TRIANGLES, 0, static_cast<u32>(vertices_n));
        } else {
            glDrawElements(GL_TRIANGLES, static_cast<u32>(indices_n), GL_UNSIGNED_INT, nullptr);
        }
    }
};

struct StaticMesh : Mesh {
    StaticMesh(const std::initializer_list<AttribDesc> &attribs) : Mesh(attribs) {}

    void set_index_data(const void *data, size_t size) {
        Mesh::set_index_data(data, size, static_cast<u32>(GL_STATIC_DRAW));
    }
    void set_index_data(const auto &list) {
        Mesh::set_index_data(list.data(), list.size() * sizeof(list[0]), static_cast<u32>(GL_STATIC_DRAW));
    }
    void use_ibo(auto &&...args) {
        Mesh::use_ibo(args..., static_cast<u32>(GL_STATIC_DRAW));
    }

    void set_data(const void *data, size_t size) {
        Mesh::set_data(data, size, static_cast<u32>(GL_STATIC_DRAW));
    }
    void set_data(const auto &list) {
        Mesh::set_data(list.data(), list.size() * sizeof(list[0]), static_cast<u32>(GL_STATIC_DRAW));
    }
};

struct DynamicMesh : Mesh {
    uint8_t *mapped_vbo;
    uint8_t *mapped_ibo;

    DynamicMesh(const std::initializer_list<AttribDesc> &attribs) : Mesh(attribs) {}

    void set_index_data(const void *data, size_t size) {
        Mesh::set_index_data(data, size, static_cast<u32>(GL_DYNAMIC_DRAW));
    }
    void set_index_data(const auto &list) {
        Mesh::set_index_data(list.data(), list.size() * sizeof(list[0]), static_cast<u32>(GL_DYNAMIC_DRAW));
    }
    void use_ibo(auto &&...args) {
        Mesh::use_ibo(args..., static_cast<u32>(GL_DYNAMIC_DRAW));
    }

    void set_data(const void *data, size_t size) {
        Mesh::set_data(data, size, static_cast<u32>(GL_DYNAMIC_DRAW));
    }
    void set_data(const auto &list) {
        Mesh::set_data(list.data(), list.size() * sizeof(list[0]), static_cast<u32>(GL_DYNAMIC_DRAW));
    }

    void begin() {
        mapped_vbo = reinterpret_cast<uint8_t *>(glMapNamedBuffer(vbo_id, GL_READ_WRITE));
        vertices_n = 0;
        if (max_indices_n > 0) {
            mapped_ibo = reinterpret_cast<uint8_t *>(glMapNamedBuffer(ibo_id, GL_READ_WRITE));
            indices_n = 0;
        }
    }
    void push_vertices(const auto &list) {
        const auto data_size = list.size() * sizeof(list[0]);
        std::memcpy(mapped_vbo, list.data(), data_size);
        mapped_vbo += data_size;
        vertices_n += data_size / vertex_size;
    }
    void push_indices(const auto &list) {
        const auto data_size = list.size() * sizeof(list[0]);
        std::memcpy(mapped_ibo, list.data(), data_size);
        mapped_ibo += data_size;
        indices_n += data_size / sizeof(u32);
    }
    void end() {
        glUnmapNamedBuffer(vbo_id);
        if (max_indices_n > 0) {
            glUnmapNamedBuffer(ibo_id);
        }
    }
};

struct StaticModel {
    std::vector<Mesh> meshes;

    StaticModel(const std::initializer_list<AttribDesc> &attribs) {
        meshes.push_back(attribs);
    }

    StaticModel(const std::filesystem::path &path) {
        auto assimp_model = AssimpModel(path);
        for (auto &node : assimp_model.source->nodes) {
            meshes.push_back({
                {.size = 3, .type = GL_FLOAT},
                {.size = 3, .type = GL_FLOAT},
                {.size = 2, .type = GL_FLOAT},
            });
            auto &m = meshes.back();
            m.set_data(node.vertices, static_cast<u32>(GL_STATIC_DRAW));
            m.use_ibo(node.indices, static_cast<u32>(GL_STATIC_DRAW));
            m.modl_mat = node.modl_mat;
        }
    }

    void draw(const auto &fn) {
        for (const auto &info : meshes) {
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
