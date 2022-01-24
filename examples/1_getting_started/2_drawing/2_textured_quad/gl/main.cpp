#include <cuiui/cuiui.hpp>
#include <cuiui/platform/defaults.hpp>
#include <coel/opengl/core.hpp>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include "../0_common/data.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
namespace cuiui_default = cuiui::platform::defaults;

struct Image {
    std::vector<uint8_t> pixels;
    int32_t size_x, size_y, channels;
};

Image load_image(const char *const filepath) {
    Image result;
    stbi_set_flip_vertically_on_load(false);
    stbi_uc *data = stbi_load(filepath, &result.size_x, &result.size_y, &result.channels, 0);
    result.pixels.resize(static_cast<size_t>(result.size_x * result.size_y * result.channels));
    for (size_t i = 0; i < result.pixels.size(); ++i)
        result.pixels[i] = data[i];
    stbi_image_free(data);
    return result;
}

int main() {
    cuiui_default::Context ui;
    coel::opengl::Context gl_ctx;
    uint32_t vao_id = std::numeric_limits<uint32_t>::max();
    uint32_t vbo_id = std::numeric_limits<uint32_t>::max();
    uint32_t ibo_id = std::numeric_limits<uint32_t>::max();
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
        glVertexArrayAttribFormat(vao_id, 0, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(float));
        glEnableVertexArrayAttrib(vao_id, 1);
        glVertexArrayAttribBinding(vao_id, 1, 0);
        glVertexArrayAttribFormat(vao_id, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
        glVertexArrayVertexBuffer(vao_id, 0, vbo_id, 0 * sizeof(float), sizeof(float) * 4);
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

        Image image = load_image("examples/1_getting_started/2_drawing/2_textured_quad/0_common/test.png");
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
        glViewport(0, 0, w->size.x, w->size.y);
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program_id);
        glBindVertexArray(vao_id);
        glUniform1i(1, 0);
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, tex_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        gl_ctx.swap_buffers();
    }
    glDeleteProgram(shader_program_id);
    glDeleteBuffers(1, &vbo_id);
    glDeleteVertexArrays(1, &vao_id);
}
