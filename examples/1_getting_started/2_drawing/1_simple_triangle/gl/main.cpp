#include <cuiui/cuiui.hpp>
#include <cuiui/platform/defaults.hpp>
#include <coel/opengl/core.hpp>
#include <glad/glad.h>
#include <iostream>
#include "../0_common.hpp"
namespace cuiui_default = cuiui::platform::defaults;
int main() {
    cuiui_default::Context ui;
    coel::opengl::Context gl_ctx;
    uint32_t vao_id = std::numeric_limits<uint32_t>::max();
    uint32_t vbo_id = std::numeric_limits<uint32_t>::max();
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
        glNamedBufferData(vbo_id, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
        glEnableVertexArrayAttrib(vao_id, 0);
        glVertexArrayAttribBinding(vao_id, 0, 0);
        glVertexArrayAttribFormat(vao_id, 0, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(float));
        glEnableVertexArrayAttrib(vao_id, 1);
        glVertexArrayAttribBinding(vao_id, 1, 0);
        glVertexArrayAttribFormat(vao_id, 1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
        glVertexArrayVertexBuffer(vao_id, 0, vbo_id, 0 * sizeof(float), sizeof(float) * 5);
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
    while (true) {
        auto w = ui.window({.id = "w"});
        if (w->should_close)
            break;
        gl_ctx.make_current();
        glViewport(0, 0, w->size.x, w->size.y);
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program_id);
        glBindVertexArray(vao_id);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        gl_ctx.swap_buffers();
    }
    glDeleteProgram(shader_program_id);
    glDeleteBuffers(1, &vbo_id);
    glDeleteVertexArrays(1, &vao_id);
}
