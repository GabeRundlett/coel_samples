#include <cuiui/cuiui.hpp>
#include <cuiui/platform/defaults.hpp>
#include <coel/opengl/core.hpp>
#include <glad/glad.h>
#include "../0_common/data.hpp"
namespace cuiui_default = cuiui::platform::defaults;

int main() {
    cuiui_default::Context ui;
    coel::opengl::Context gl_ctx;
    uint32_t vao_id = std::numeric_limits<uint32_t>::max();
    uint32_t vbo_id = std::numeric_limits<uint32_t>::max();
    uint32_t ibo_id = std::numeric_limits<uint32_t>::max();
    uint32_t ubo_id = std::numeric_limits<uint32_t>::max();
    uint32_t tex_id = std::numeric_limits<uint32_t>::max();
    uint32_t shader_program_id = std::numeric_limits<uint32_t>::max();
    {
        auto w = ui.window({.id = "w", .size = {400, 400}});
        gl_ctx.attach(w->hwnd);
        gl_ctx.make_current();
        gladLoadGL();
    }
    {
        gl_ctx.make_current();
        glCreateVertexArrays(1, &vao_id);
        glCreateBuffers(1, &vbo_id);
        glNamedBufferData(vbo_id, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
        glCreateBuffers(1, &ibo_id);
        glNamedBufferData(ibo_id, sizeof(indices), indices.data(), GL_STATIC_DRAW);
        glEnableVertexArrayAttrib(vao_id, 0);
        glVertexArrayAttribBinding(vao_id, 0, 0);
        glVertexArrayAttribFormat(vao_id, 0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float));
        glEnableVertexArrayAttrib(vao_id, 1);
        glVertexArrayAttribBinding(vao_id, 1, 0);
        glVertexArrayAttribFormat(vao_id, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
        glVertexArrayVertexBuffer(vao_id, 0, vbo_id, 0 * sizeof(float), sizeof(float) * 5);
        glCreateBuffers(1, &ubo_id);
        glNamedBufferData(ubo_id, sizeof(uniforms), &uniforms, GL_DYNAMIC_DRAW);
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
        int32_t internalformat = 0;
        uint32_t format = 0;
        uint32_t type = GL_UNSIGNED_BYTE;
        switch (image.channels) {
        case 3: internalformat = GL_RGB8, format = GL_RGB; break;
        case 4: internalformat = GL_RGBA8, format = GL_RGBA; break;
        }
        glGenTextures(1, &tex_id);
        glBindTexture(GL_TEXTURE_2D, tex_id);
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, image.size_x, image.size_y, 0, format, type, image.pixels.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    while (true) {
        auto w = ui.window({.id = "w"});
        if (w->should_close)
            break;
        gl_ctx.make_current();
        game_state.update(w);
        auto &temp_uniforms = *reinterpret_cast<Uniforms*>(glMapNamedBuffer(ubo_id, GL_READ_WRITE));
        temp_uniforms = {
            .proj = scale(f32mat4::identity(), {static_cast<float>(w->size.y) / w->size.x, 1.0f, 0.01f}),
            // .proj = perspective(radians(45.0f), static_cast<float>(w->size.x) / static_cast<float>(w->size.y), 0.01f, 100.0f),
            .view = translate(rotate(rotate(uniforms.view, game_state.rot_y, {0, 1, 0}), game_state.rot_x, {1, 0, 0}), {game_state.pos_x, game_state.pos_y, game_state.pos_z}),
        };
        temp_uniforms.view = scale(temp_uniforms.view, {1, -1, 1});
        glUnmapNamedBuffer(ubo_id);

        glViewport(0, 0, w->size.x, w->size.y);
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program_id);
        glUniformBlockBinding(shader_program_id, 0, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_id);
        glBindVertexArray(vao_id);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, tex_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
        gl_ctx.swap_buffers();
    }
    glDeleteProgram(shader_program_id);
    glDeleteBuffers(1, &vbo_id);
    glDeleteVertexArrays(1, &vao_id);
}
