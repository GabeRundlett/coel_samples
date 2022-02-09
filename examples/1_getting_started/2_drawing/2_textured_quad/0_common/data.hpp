#pragma once

#include <array>
#include <chrono>
#include <iostream>
#include <thread>

#include <cuiui/math/types.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct Image {
    std::vector<uint8_t> pixels;
    int32_t size_x, size_y, channels;
};

struct Uniforms {
    f32mat4 proj;
    f32mat4 view;
};

struct GameState {
    float pos_x = 0.0f;
    float pos_y = 0.0f;
    float pos_z = 0.0f;
    float rot_x = 0.0f;
    float rot_y = 0.0f;
    bool move_nx = false, move_px = false;
    bool move_ny = false, move_py = false;
    bool move_nz = false, move_pz = false;
    // bool rot_ny = false, rot_py = false;
    float prev_mouse_x = 0.0f, prev_mouse_y = 0.0f;
    bool paused = false;

    std::chrono::high_resolution_clock::time_point prev_time = std::chrono::high_resolution_clock::now();
    static constexpr auto FRAMETIMES_N = 20;
    std::array<double, FRAMETIMES_N> last_n_frametimes{};
    size_t frametimes_i = 0;

    void update(cuiui::WindowHandle<cuiui::platform::defaults::Window> &w) {
        while (!w->events.empty()) {
            auto event = w->events.front();
            switch (event.type) {
            case cuiui::EventType::KeyEvent: {
                auto &key_event = std::get<cuiui::KeyEvent>(event.data);
                // std::cout << key_event.key << "\n";
                if (key_event.key == 'W')
                    move_nz = key_event.action != 0;
                if (key_event.key == 'S')
                    move_pz = key_event.action != 0;
                if (key_event.key == 'A')
                    move_px = key_event.action != 0;
                if (key_event.key == 'D')
                    move_nx = key_event.action != 0;
                if (key_event.key == 32)
                    move_py = key_event.action != 0;
                if (key_event.key == 16)
                    move_ny = key_event.action != 0;
                // if (key_event.key == 'Q')
                //     rot_ny = key_event.action != 0;
                // if (key_event.key == 'E')
                //     rot_py = key_event.action != 0;
                if (key_event.key == 27 && key_event.action == 0)
                    paused = !paused;
            } break;
            case cuiui::EventType::MouseMotionEvent: {
                auto &mouse_move_event = std::get<cuiui::MouseMotionEvent>(event.data);
                float mouse_x = static_cast<float>(mouse_move_event.pos.x);
                float mouse_y = static_cast<float>(mouse_move_event.pos.y);
                float diff_x = mouse_x - prev_mouse_x;
                float diff_y = mouse_y - prev_mouse_y;
                if (!paused) {
                    prev_mouse_x = static_cast<float>(w->size.x) / 2;
                    prev_mouse_y = static_cast<float>(w->size.y) / 2;
                    rot_y -= diff_x / 1000;
                    rot_x += diff_y / 1000;
                    w->set_mouse_pos(w->size.x / 2, w->size.y / 2);
                }
            } break;
            default: break;
            }
            w->events.pop();
        }
        if (move_nx)
            pos_x -= 0.01f;
        if (move_px)
            pos_x += 0.01f;
        if (move_ny)
            pos_y -= 0.01f;
        if (move_py)
            pos_y += 0.01f;
        if (move_nz)
            pos_z -= 0.01f;
        if (move_pz)
            pos_z += 0.01f;
        // if (rot_ny)
        //     rot_y -= 0.01f;
        // if (rot_py)
        //     rot_y += 0.01f;

        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(now - prev_time).count();
        prev_time = now;
        last_n_frametimes[frametimes_i] = elapsed;
        double last_n_frametimes_avg = 0.0;
        for (auto t : last_n_frametimes)
            last_n_frametimes_avg += t;
        last_n_frametimes_avg *= 1.0 / FRAMETIMES_N;
        frametimes_i++;
        if (frametimes_i == FRAMETIMES_N)
            frametimes_i = 0;
        std::cout << "frame took " << elapsed * 1000 << "ms (avg: " << last_n_frametimes_avg << ", " << 1.0 / last_n_frametimes_avg << " FPS)\n";
    }
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

const char *const vert_src = R"(
#version 460 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_tex;
layout(location = 0) out vec2 v_tex;
layout(set = 0, binding = 0) uniform Ubo {
    mat4 proj;
    mat4 view;
} u;
void main() {
    v_tex = a_tex;
    gl_Position = u.proj * u.view * vec4(a_pos, 1);
})";

const char *const frag_src = R"(
#version 460 core
layout(location = 0) in vec2 v_tex;
layout(location = 0) out vec4 o_col;
layout(binding = 1) uniform sampler2D tex;
void main() {
    o_col = texture(tex, v_tex);
})";

std::array<float, 24 * 5> vertices{
    // clang-format off
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, // top    left
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, // bottom right
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f, // top    right

    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, // bottom left
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f, // bottom right
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, // top    left
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f, // top    right

    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, // bottom left
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, // bottom right
    -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, // top    left
     0.5f, -0.5f,  0.5f,   1.0f, 1.0f, // top    right

    -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, // bottom left
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, // top    left
     0.5f,  0.5f, -0.5f,   1.0f, 0.0f, // bottom right
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f, // top    right
     
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, // bottom left
    -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, // bottom right
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, // top    left
    -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, // top    right

     0.5f, -0.5f, -0.5f,   0.0f, 0.0f, // bottom left
     0.5f,  0.5f, -0.5f,   0.0f, 1.0f, // top    left
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f, // bottom right
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f, // top    right
    // clang-format on
};

std::array<uint32_t, 36> indices{
    // clang-format off
    0 + 4*0, 1 + 4*0, 2 + 4*0,
    1 + 4*0, 3 + 4*0, 2 + 4*0,

    0 + 4*1, 1 + 4*1, 2 + 4*1,
    1 + 4*1, 3 + 4*1, 2 + 4*1,

    0 + 4*2, 1 + 4*2, 2 + 4*2,
    1 + 4*2, 3 + 4*2, 2 + 4*2,

    0 + 4*3, 1 + 4*3, 2 + 4*3,
    1 + 4*3, 3 + 4*3, 2 + 4*3,

    0 + 4*4, 1 + 4*4, 2 + 4*4,
    1 + 4*4, 3 + 4*4, 2 + 4*4,

    0 + 4*5, 1 + 4*5, 2 + 4*5,
    1 + 4*5, 3 + 4*5, 2 + 4*5,
    // clang-format on
};

Uniforms uniforms{
    .proj = scale(f32mat4::identity(), {1.0f, 1.0f, 1.0f}),
    .view = scale(f32mat4::identity(), {1.0f, 1.0f, 1.0f}),
};

Image image = load_image("examples/1_getting_started/2_drawing/2_textured_quad/0_common/test.png");

GameState game_state;
