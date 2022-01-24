#pragma once

#include <glad/glad.h>
#include <coel/opengl/context.hpp>
#include <array>
#include <iostream>

#include "app_common.hpp"

struct OpenGLApp {
    coel::opengl::Context gl_ctx;

    uint32_t scene_vao_id, scene_vbo_id;
    uint32_t scene_shader_program_id;

    uint32_t ui_vao_id, ui_vbo_id;
    uint32_t ui_shader_program_id;

    uint32_t ubo_id;

    void init(AppState &, WindowHandle w) {
        #if COEL_USE_WIN32
        gl_ctx.attach(w->hwnd);
        #elif COEL_USE_X11
        gl_ctx.attach(w->xid);
        #endif
        gl_ctx.make_current();

        gladLoadGL();

        glCreateBuffers(1, &ubo_id);
        uniforms.view = scale(f32mat4::identity(), {1, -1, 1});
        uniforms.ui_view = scale(translate(f32mat4::identity(), {0, 0, 0}), {1, -1, 1});

        glNamedBufferData(ubo_id, sizeof(uniforms), &uniforms, GL_DYNAMIC_DRAW);

        glCreateVertexArrays(1, &scene_vao_id);
        glCreateBuffers(1, &scene_vbo_id);
        glNamedBufferData(scene_vbo_id, sizeof(scene_vertices), scene_vertices.data(), GL_DYNAMIC_DRAW);
        glEnableVertexArrayAttrib(scene_vao_id, 0);
        glVertexArrayAttribBinding(scene_vao_id, 0, 0);
        glVertexArrayAttribFormat(scene_vao_id, 0, 2, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(scene_vao_id, 1);
        glVertexArrayAttribBinding(scene_vao_id, 1, 0);
        glVertexArrayAttribFormat(scene_vao_id, 1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
        glVertexArrayVertexBuffer(scene_vao_id, 0, scene_vbo_id, 0 * sizeof(float), sizeof(SceneVertex));

        glCreateVertexArrays(1, &ui_vao_id);
        glCreateBuffers(1, &ui_vbo_id);
        glNamedBufferData(ui_vbo_id, sizeof(ui_vertices), ui_vertices.data(), GL_DYNAMIC_DRAW);
        glEnableVertexArrayAttrib(ui_vao_id, 0);
        glVertexArrayAttribBinding(ui_vao_id, 0, 0);
        glVertexArrayAttribFormat(ui_vao_id, 0, 2, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(ui_vao_id, 1);
        glVertexArrayAttribBinding(ui_vao_id, 1, 0);
        glVertexArrayAttribFormat(ui_vao_id, 1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
        glVertexArrayVertexBuffer(ui_vao_id, 0, ui_vbo_id, 0 * sizeof(float), sizeof(UiVertex));

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
        {
            scene_shader_program_id = glCreateProgram();
            auto vert_shader_id = attach_shader(scene_shader_program_id, GL_VERTEX_SHADER, scene_vert_src);
            auto frag_shader_id = attach_shader(scene_shader_program_id, GL_FRAGMENT_SHADER, scene_frag_src);
            glLinkProgram(scene_shader_program_id);
            glDetachShader(scene_shader_program_id, vert_shader_id);
            glDetachShader(scene_shader_program_id, frag_shader_id);
            glDeleteShader(vert_shader_id);
            glDeleteShader(frag_shader_id);
        }
        {
            ui_shader_program_id = glCreateProgram();
            auto vert_shader_id = attach_shader(ui_shader_program_id, GL_VERTEX_SHADER, ui_vert_src);
            auto frag_shader_id = attach_shader(ui_shader_program_id, GL_FRAGMENT_SHADER, ui_frag_src);
            glLinkProgram(ui_shader_program_id);
            glDetachShader(ui_shader_program_id, vert_shader_id);
            glDetachShader(ui_shader_program_id, frag_shader_id);
            glDeleteShader(vert_shader_id);
            glDeleteShader(frag_shader_id);
        }
    }
    void deinit(AppState &, WindowHandle w) {
        gl_ctx.make_current();
        glDeleteProgram(scene_shader_program_id);
        glDeleteProgram(ui_shader_program_id);
        glDeleteBuffers(1, &ui_vbo_id);
        glDeleteVertexArrays(1, &ui_vao_id);
        glDeleteBuffers(1, &scene_vbo_id);
        glDeleteVertexArrays(1, &scene_vao_id);
        #if COEL_USE_WIN32
        gl_ctx.detach(w->hwnd);
        #elif COEL_USE_X11
        gl_ctx.detach(w->xid);
        #endif
    }
    bool update(AppState &app_state, WindowHandle) {
        gl_ctx.make_current();

        glViewport(0, 0, static_cast<GLsizei>(app_state.size_x), static_cast<GLsizei>(app_state.size_y));
        glClearColor(0.08f, 0.08f, 0.16f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(scene_shader_program_id);
        glUniformBlockBinding(scene_shader_program_id, 0, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_id);
        {
            SceneVertex *mapped_vertices = static_cast<SceneVertex *>(glMapNamedBuffer(scene_vbo_id, GL_READ_WRITE));
            for (size_t i = 0; i < scene_vertices.size(); ++i)
                mapped_vertices[i].pos = scene_vertices[i].pos + f32vec2{app_state.mouse_x, app_state.mouse_y};
            glUnmapNamedBuffer(scene_vbo_id);
        }
        glBindVertexArray(scene_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, scene_vertices.size());

        glUseProgram(ui_shader_program_id);
        glUniformBlockBinding(ui_shader_program_id, 0, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_id);
        glUniformBlockBinding(ui_shader_program_id, 0, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_id);
        glBindVertexArray(ui_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, ui_vertices.size());

        gl_ctx.swap_buffers();
        return true;
    }
};
