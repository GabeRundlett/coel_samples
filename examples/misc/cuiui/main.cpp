#include "window.hpp"
#include "context.hpp"
#include <1_getting_started/2_drawing/0_common/render.hpp>

#include <iostream>
// #include <string>
#include <cuiui/math/utility.hpp>

struct UiRenderer {
    Shader ui_shader = Shader(
        // clang-format off
        R"glsl(
            #version 460 core
            layout(location = 0) in vec2 a_pos;
            layout(location = 1) in vec2 a_tex;
            layout(location = 2) in vec4 a_col;
            layout(location = 3) in vec4 a_props;
            layout(location = 0) out vec2 v_tex;
            layout(location = 1) out vec4 v_col;
            layout(location = 2) out vec4 v_props;
            uniform mat4 modl_mat;
            void main() {
                v_tex = a_tex;
                v_col = a_col;
                v_props = a_props;
                gl_Position = modl_mat * vec4(a_pos, 0, 1);
            }
        )glsl",
        R"glsl(
            #version 460 core
            layout(location = 0) in vec2 v_tex;
            layout(location = 1) in vec4 v_col;
            layout(location = 2) in vec4 v_props;
            layout(location = 0) out vec4 o_col;

            void draw_shadow() {
                o_col = vec4(v_col);
                float r = v_props.y;
                vec2 size = v_props.zw;
                vec2 uv = v_tex * size;
                float dist = 0.0;
                if (uv.x > size.x - r) {
                    if (uv.y > size.y - r) {
                        dist = length(uv - size + vec2(r)) / r;
                    } else if (uv.y < r) {
                        dist = length(uv - vec2(size.x - r, r)) / r;
                    } else {
                        dist = (uv.x - size.x + r) / r;
                    }
                } else if (uv.x < r) {
                    if (uv.y > size.y - r) {
                        dist = length(uv - vec2(r, size.y - r)) / r;
                    } else if (uv.y < r) {
                        dist = length(uv - vec2(r)) / r;
                    } else {
                        dist = (r - uv.x) / r;
                    }
                } else {
                    if (uv.y > size.y - r) {
                        dist = (uv.y - size.y + r) / r;
                    } else if (uv.y < r) {
                        dist = (r - uv.y) / r;
                    } else {
                        
                    }
                }
                o_col.a = 1.0 - dist;
                o_col.a = o_col.a * o_col.a * 0.2;
            }

            vec3 hsv2rgb(vec3 c) {
                vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
                vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
                return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
            }

            void main() {
                int type = int(v_props.r);
                switch (type) {
                case 0: {
                    o_col = vec4(v_col);
                } break;
                case 1: {
                    draw_shadow();
                } break;
                case 2: {
                    o_col = vec4(v_col);
                    vec2 size = v_props.zw;
                    vec2 uv = v_tex * size;
                    o_col.a = 1.0 - clamp((length(v_tex - 0.5) * size.x - (size.x * 0.5)) * 2.0 + 0.5, 0, 1);
                } break;
                case 3: {
                    // o_col = vec4(mix(vec3(1), vec3(1, 0, 0), v_tex.y) * v_tex.x, 1);
                    o_col = vec4(hsv2rgb(vec3(v_props.g, 1 - v_tex.xy)), 1.0);
                } break;
                }
            }
        )glsl"
        // clang-format on
    );

    f32mat4 modl_mat;
    Uniform ui_shader_modl_mat = ui_shader.uniform("modl_mat");

    DynamicMesh ui_mesh = DynamicMesh({
        {.size = 2, .type = GL_FLOAT},
        {.size = 2, .type = GL_FLOAT},
        {.size = 4, .type = GL_FLOAT},
        {.size = 4, .type = GL_FLOAT},
    });

    UiRenderer() {
        // TODO: make this vertex size thing optionally a part of the mesh's type
        constexpr auto V_SIZE = (2 + 2 + 4 + 4) * sizeof(f32);
        constexpr auto I_SIZE = sizeof(u32);
        constexpr auto MAX_QUADS = 1000;
        ui_mesh.set_data(nullptr, 4 * V_SIZE * MAX_QUADS);
        ui_mesh.use_ibo(nullptr, 6 * I_SIZE * MAX_QUADS);
    }

    void update(f32vec2 viewport_size) {
        modl_mat = scale(translate(f32mat4::identity(), {-1.0f, 1.0f, 0.0f}), {2.0f / static_cast<f32>(viewport_size[0]), -2.0f / static_cast<f32>(viewport_size[1]), 1.0f});
        glViewport(0, 0, static_cast<u32>(viewport_size.x), static_cast<u32>(viewport_size.y));
    }

    void render(auto &ui) {
        ui_mesh.begin();
        auto push_render_element = [&](const auto &e) {
            auto n = static_cast<u32>(ui_mesh.vertices_n);
            ui_mesh.push_indices(std::array<u32, 6>{
                // clang-format off
                0 + n, 1 + n, 2 + n,
                1 + n, 3 + n, 2 + n,
                // clang-format on
            });
            ui_mesh.push_vertices(std::array<f32, 12 * 4>{
                // clang-format off
                e.rect.p0[0], e.rect.p0[1], 0.0f, 0.0f, e.col[0], e.col[1], e.col[2], e.col[3], e.props[0], e.props[1], e.props[2], e.props[3],
                e.rect.p0[0], e.rect.p1[1], 0.0f, 1.0f, e.col[0], e.col[1], e.col[2], e.col[3], e.props[0], e.props[1], e.props[2], e.props[3],
                e.rect.p1[0], e.rect.p0[1], 1.0f, 0.0f, e.col[0], e.col[1], e.col[2], e.col[3], e.props[0], e.props[1], e.props[2], e.props[3],
                e.rect.p1[0], e.rect.p1[1], 1.0f, 1.0f, e.col[0], e.col[1], e.col[2], e.col[3], e.props[0], e.props[1], e.props[2], e.props[3],
                // clang-format on
            });
        };
        for (const auto &e : ui.render_elements)
            push_render_element(e);
        ui_mesh.end();

        ui_shader.use();
        ui_shader_modl_mat.send_mat4(modl_mat);
        ui_mesh.draw();
    }
};

using namespace cuiui::components;

int main() {
    cuiui::Context ui{};
    // size_t b_counter = 0;
    // std::string counter_str = "";

    ui.submit<Window>({.id = "main_window", .init{.dim{600, 600}}});
    UiRenderer ui_renderer;

    f32vec3 clear_col{0, 0, 0};

    while (true) {
        ui.new_frame();
        {
            auto w = ui.submit<Window>({.id = "main_window"});
            if (ui.get(w).should_close)
                break;
            // auto bg = ui.submit<Quad>({.id = "bg", .init{.pos{0.0f, 0.0f}, .col = ui.colors.background}});
            // ui.get(bg).size = ui.get(w).dim;
            ui_renderer.update(ui.get(w).dim);
            // ui.submit<Text>({.id = "tx0", .content = "Hello, World!"});
            auto cb = ui.submit<Checkbox>({.id = "cb0"});
            ui.submit<Window>({.id = "sw1", .init{.pos{100, 100}, .dim{250, 380}}});
            ui.submit<Slider>({.id = "s0"});
            if (ui.get(cb).checked) {
                ui.submit<Window>({.id = "sw0", .init{.pos{100, 100}, .dim{250, 380}}});
                auto b = ui.submit<Button>({.id = "sb0"});
                if (ui.get(b).pressed)
                    ui.get(cb).checked = false;
            }
            ui.submit<Slider>({.id = "s1"});
            ui.submit<Checkbox>({.id = "cb1"});
            auto col = ui.submit<ColorEditRGB>({.id = "ce0"});
            clear_col = ui.get(col).rgb;

            auto b = ui.submit<Button>({.id = "b0"});
            if (ui.get(b).pressed) {
                std::cout << "pressed ";
                // b_counter++;
                // counter_str.clear();
                // std::format_to(std::back_inserter(counter_str), "Counter = {}", b_counter);
            }

            // ui.submit<Text>({.content = counter_str});
        }
        ui.render_frame();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(clear_col[0], clear_col[1], clear_col[2], 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ui_renderer.render(ui);
    }
}
