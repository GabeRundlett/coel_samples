#include <cuiui/cuiui.hpp>
#include <cuiui/platform/defaults.hpp>
#include <coel/opengl/core.hpp>
#include <glad/glad.h>
namespace cuiui_default = cuiui::platform::defaults;

#include <array>
#include <chrono>
#include <iostream>
#include <thread>
#include <cuiui/math/types.hpp>

#include "math.hpp"
#include <numbers>

const uint32_t TILE_NX = 16, TILE_NY = 16;
i32vec2 CHUNK_POS = {-8, -8};

struct Storage {
    f32mat4 proj;
    f32mat4 view;
    f32vec2 mouse;
    f32vec2 ray_pos;
    f32vec2 ray_dir;
    f32vec2 points[501];
    uint32_t points_n;
    uint32_t tiles[TILE_NX * TILE_NY];
};

const char *const vert_src = R"(
#version 460 core
layout(location = 0) in vec2 a_pos;
layout(location = 0) out vec2 v_tex;
const uint TILE_NX = 16, TILE_NY = 16, TILE_NZ = 16;
const ivec2 CHUNK_POS = ivec2(-8);
layout(std430, binding = 3) buffer Buf {
    mat4 proj;
    mat4 view;
    vec2 mouse;
    vec2 ray_pos;
    vec2 ray_dir;
    vec2 points[501];
    uint points_n;
    uint tiles[TILE_NX * TILE_NY];
};
void main() {
    vec4 pos = vec4(a_pos, 0, 1);
    v_tex = (proj * view * pos).xy;
    gl_Position = pos;
})";

const char *const frag_src = R"(
#version 460 core
layout(location = 0) in vec2 v_tex;
layout(location = 0) out vec4 o_col;
const uint TILE_NX = 16, TILE_NY = 16, TILE_NZ = 16;
const ivec2 CHUNK_POS = ivec2(-8);
layout(std430, binding = 3) buffer Buf {
    mat4 proj;
    mat4 view;
    vec2 mouse;
    vec2 ray_pos;
    vec2 ray_dir;
    vec2 points[501];
    uint points_n;
    uint tiles[TILE_NX * TILE_NY];
};

void fill(bool inside, vec3 color) {
    if (inside) o_col.rgb = color;
}
void overlay(bool inside, vec3 color) {
    if (inside) o_col.rgb += color;
}

bool axis(vec2 origin, float thickness) {
    vec2 a = abs(v_tex - origin);
    return a.x < thickness || a.y < thickness;
}

bool gridlines(float spacing, float thickness) {
    vec2 f = fract(v_tex / spacing);
    float nts = thickness / spacing;
    float its = 1.0 - nts;
    return f.x < nts || f.y < nts || f.x > its || f.y > its;
}

bool point(vec2 p, float r) {
    float d2 = dot(p - v_tex, p - v_tex);
    return d2 < r * r;
}

bool line(vec2 p0, vec2 p1, float thickness) {
    vec2 uv = v_tex;
    float a = abs(distance(p0, uv));
    float b = abs(distance(p1, uv));
    float c = abs(distance(p0, p1));
    if (a >= c || b >= c)
        return point(p0, thickness) || point(p1, thickness);
    float p = (a + b + c) * 0.5;
    float m = 2 / c * sqrt(p * (p - a) * (p - b) * (p - c) + 0.00000001);
    return m < thickness;
}

bool arrow(vec2 p0, vec2 p1, float thickness, float head_size, float pointiness) {
    vec2 t0 = normalize(vec2( p1.y, -p1.x) / pointiness + p1 * pointiness) * head_size;
    vec2 t1 = normalize(vec2(-p1.y,  p1.x) / pointiness + p1 * pointiness) * head_size;
    return line(p0, p0 + p1, thickness) ||
           line(p0 + p1, p0 + p1 - t0, thickness) ||
           line(p0 + p1, p0 + p1 - t1, thickness);
}

bool cross(vec2 p, float size, float thickness) {
    vec2 t0 = vec2(size, 0);
    vec2 t1 = vec2(0, size);
    return line(p - t0, p + t0, thickness) ||
           line(p - t1, p + t1, thickness);
}

bool rect(vec2 p0, vec2 p1) {
    vec2 pmin = min(p0, p1);
    vec2 pmax = max(p0, p1);
    return v_tex.x >= pmin.x && v_tex.y >= pmin.y && v_tex.x <= pmax.x && v_tex.y <= pmax.y;
}

void main() {
    vec2 mouse_diff = mouse - v_tex;
    vec2 center = (proj * view * vec4(0, 0, 0, 1)).xy;
    o_col = vec4(0.1, 0.1, 0.1, 1);
    for (uint yi = 0; yi < TILE_NY; ++yi) {
        for (uint xi = 0; xi < TILE_NX; ++xi) {
            vec2 tile_p = vec2(xi, yi) + vec2(CHUNK_POS);
            if (tiles[xi + yi * TILE_NX] != 0)
                fill(rect(tile_p, tile_p + 1), vec3(0.5));
            else
                fill(rect(tile_p, tile_p + 1), vec3(0.06));
        }
    }
    fill(gridlines(1, 0.02), vec3(0.2));
    fill(gridlines(16, 0.04), vec3(0.3, 0.2, 0.2));
    fill(axis(vec2(0, 0), 0.04), vec3(0.7, 0.3, 0.3));
    fill(point(mouse, 0.06), vec3(0, 0, 1));
    for (uint i = 0; i < points_n; i++)
        overlay(arrow(ray_pos, points[i]-ray_pos, 0.01, 0.0, 1.0), vec3(0.12, 0.1, 0.05) * 2);
    // for (uint i = 0; i < points_n; ++i)
    //     fill(point(points[i], 0.06), vec3(1, 0, 0));
    overlay(point(ray_pos, 0.06), vec3(0.2, 0.3, 0.0));
    overlay(point(ray_pos + ray_dir, 0.06), vec3(0.2, 0.3, 0.0));
    overlay(cross(center, 0.1, 0.02), vec3(0.2));
})";

std::array<float, 4 * 2> vertices{
    // clang-format off
    -1.0f, -1.0f, // bottom left
    -1.0f,  1.0f, // top    left
     1.0f, -1.0f, // bottom right
     1.0f,  1.0f, // top    right
    // clang-format on
};

std::array<uint32_t, 6> indices{
    // clang-format off
    0, 1, 2,
    1, 3, 2,
    // clang-format on
};

float zoom;
f32vec2 view_pos;
Storage storage;

bool grab_flag = false;
f32vec2 grab_mouse_pos;
f32vec2 grab_view_pos;
bool ctrl_pressed = false;
f32vec2 *grabbed_point = nullptr;

f32vec2 mouse_ndc, mouse_view;
float aspect;

void raycast_scene() {
    storage.points_n = 0;

    for (int i = 0; i < 501; ++i) {
        auto rot_dir4 = f32vec4{storage.ray_dir[0], storage.ray_dir[1], 0, 0};
        rot_dir4 = rot_dir4 * rotate(f32mat4::identity(), static_cast<f32>(i) * std::numbers::pi_v<float> * 2.0f / 501, f32vec3{0, 0, 1});
        auto rot_dir = normalize(f32vec2{rot_dir4[0], rot_dir4[1]});
        auto r = raycast(
            f32vec2{storage.ray_pos[0], storage.ray_pos[1]},
            f32vec2{rot_dir[0], rot_dir[1]},
            f32vec2{(f32)CHUNK_POS[0], (f32)CHUNK_POS[1]},
            f32vec2{(f32)TILE_NX + CHUNK_POS[0], (f32)TILE_NY + CHUNK_POS[1]},
            [](auto tile_i) {
                return storage.tiles[(tile_i[0] - CHUNK_POS.x) + (tile_i[1] - CHUNK_POS.y) * TILE_NX];
            });
        // for (size_t i = 0; i < r.total_steps; ++i)
        //     storage.points[i] = {(f32)r.points[i][0], (f32)r.points[i][1]};
        // storage.points_n = r.total_steps;
        if (r.hit_surface) {
            // RaycastResult<float, 2> r_2;
            // r_2.hit_edge_i = r.hit_edge_i;
            // r_2.hit_surface = r.hit_surface;
            // r_2.points = r.points;
            // r_2.tile_index = std::array<int, 2>{r.tile_index[0], r.tile_index[1]};
            // r_2.total_steps = r.total_steps;
            auto s = get_surface_details(storage.ray_pos, rot_dir, r);
            storage.points[storage.points_n] = s.pos;
            storage.points_n++;
        }
    }
}

bool point(f32vec2 p, float r) {
    auto v = mouse_view - p;
    float d2 = v.dot(v);
    return d2 < r * r;
}

void on_grab() {
    grab_flag = true;
}

void grab(bool inside, auto on_grab_cb) {
    if (inside) {
        on_grab();
        on_grab_cb();
    }
}

void grab_item() {
    grab_mouse_pos = mouse_ndc;
    grab_view_pos = view_pos;
    for (uint32_t i = 0; i < storage.points_n; ++i) {
        grab(point(storage.points[i], 0.06f), [&]() {
            grabbed_point = &storage.points[i];
            grab_mouse_pos = mouse_view - storage.points[i];
        });
    }
    grab(point(storage.ray_pos, 0.06f), [&]() {
        grabbed_point = &storage.ray_pos;
        grab_mouse_pos = mouse_view - storage.ray_pos;
    });
    grab(point(storage.ray_dir + storage.ray_pos, 0.06f), [&]() {
        grabbed_point = &storage.ray_dir;
        grab_mouse_pos = mouse_view - (storage.ray_dir + storage.ray_pos);
    });
}

void grab_view() {
    grab_flag = true;
    grabbed_point = nullptr;
    grab_mouse_pos = mouse_ndc;
    grab_view_pos = view_pos;
}

void release() {
    grab_flag = false;
}

void drag_item() {
    auto &p = *grabbed_point;
    p = mouse_view - grab_mouse_pos;
    if (ctrl_pressed)
        p = {
            static_cast<float>(static_cast<int32_t>(p.x - 0.5f * (static_cast<float>(signbit(p.x)) * 2.0f - 1.0f))),
            static_cast<float>(static_cast<int32_t>(p.y - 0.5f * (static_cast<float>(signbit(p.y)) * 2.0f - 1.0f))),
        };
    if (grabbed_point == &storage.ray_dir)
        p = normalize(p - storage.ray_pos);
}

void drag_view() {
    view_pos = grab_view_pos - (mouse_ndc - grab_mouse_pos) * zoom * f32vec2{aspect, 1.0f};
    // if (view_pos.x < 0.0f)
    //     view_pos.x = 0.0f;
    // if (view_pos.y < -static_cast<float>(TILE_NY))
    //     view_pos.y = -static_cast<float>(TILE_NY);
    // if (view_pos.x > static_cast<float>(TILE_NX))
    //     view_pos.x = static_cast<float>(TILE_NX);
    // if (view_pos.y > 0.0f)
    //     view_pos.y = 0.0f;
}

void reset_view() {
    zoom = 16.0f;
    view_pos = {0.0f, 0.0f};
}

void reset_tiles() {
    for (uint32_t yi = 0; yi < TILE_NY; ++yi) {
        for (uint32_t xi = 0; xi < TILE_NX; ++xi) {
            auto index = xi + yi * TILE_NX;
            storage.tiles[index] = 0;
            storage.tiles[index] = static_cast<uint32_t>(rand() % 10 == 0);
            if (yi == 0 || yi == TILE_NY - 1 || xi == 0 || xi == TILE_NX - 1)
                storage.tiles[index] = 1;
        }
    }
}

int main() {
    cuiui_default::Context ui;
    coel::opengl::Context gl_ctx;
    uint32_t vao_id = std::numeric_limits<uint32_t>::max();
    uint32_t vbo_id = std::numeric_limits<uint32_t>::max();
    uint32_t ibo_id = std::numeric_limits<uint32_t>::max();
    uint32_t sbo_id = std::numeric_limits<uint32_t>::max();
    uint32_t shader_program_id = std::numeric_limits<uint32_t>::max();
    {
        auto w = ui.window({.id = "w", .size = {1200, 900}});
        gl_ctx.attach(w->hwnd);
        gl_ctx.make_current();
        gladLoadGL();
    }
    {
        glCreateVertexArrays(1, &vao_id);
        glCreateBuffers(1, &vbo_id);
        glNamedBufferData(vbo_id, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
        glCreateBuffers(1, &ibo_id);
        glNamedBufferData(ibo_id, sizeof(indices), indices.data(), GL_STATIC_DRAW);
        glEnableVertexArrayAttrib(vao_id, 0);
        glVertexArrayAttribBinding(vao_id, 0, 0);
        glVertexArrayAttribFormat(vao_id, 0, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(float));
        glVertexArrayVertexBuffer(vao_id, 0, vbo_id, 0 * sizeof(float), sizeof(float) * 2);
        glCreateBuffers(1, &sbo_id);
        glNamedBufferData(sbo_id, sizeof(Storage), nullptr, GL_DYNAMIC_DRAW);
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

    storage.ray_pos = {0.0f, 0.0f};
    storage.ray_dir = normalize(f32vec2{1.0f, 0.0f});

    storage.points[0] = {2.1f, 4.4f};
    storage.points[1] = {-2.3f, 3.5f};
    storage.points_n = 2;

    reset_view();
    reset_tiles();

    while (true) {
        auto w = ui.window({.id = "w"});
        if (w->should_close)
            break;

        auto screen_to_ndc = [&](f32vec2 p) {
            const auto wsize = f32vec2{static_cast<float>(w->size.x), static_cast<float>(w->size.y)};
            return p / wsize * f32vec2{2.0f, -2.0f} + f32vec2{-1.0f, 1.0f};
        };
        auto screen_to_view = [&](f32vec2 p) {
            const auto wsize = f32vec2{static_cast<float>(w->size.x), static_cast<float>(w->size.y)};
            const auto view_offset = f32vec2{view_pos.x / 2 / aspect - 0.5f * zoom, -view_pos.y / 2 - 0.5f * zoom};
            const auto proj_offset = f32vec2{2.0f * aspect, 2.0f};
            return (p * zoom / wsize + view_offset) * proj_offset;
        };
        aspect = static_cast<float>(w->size.x) / static_cast<float>(w->size.y);
        mouse_ndc = screen_to_ndc(w->mouse_pos);
        mouse_view = screen_to_view(w->mouse_pos);

        raycast_scene();

        while (!w->events.empty()) {
            auto &event = w->events.front();
            switch (event.type) {
            case cuiui::EventType::KeyEvent: {
                auto &e = std::get<cuiui::KeyEvent>(event.data);
                // if (e.action == 0 && e.key == 'R')
                //     raycast_scene();
                if (e.action == 0 && e.key == 'F')
                    reset_view();
                if (e.key == 16)
                    ctrl_pressed = e.action != 0;
            } break;
            case cuiui::EventType::MouseButtonEvent: {
                auto &e = std::get<cuiui::MouseButtonEvent>(event.data);
                if (e.action == 1) {
                    switch (e.key) {
                    case 0: grab_item(); break;
                    case 2: grab_view(); break;
                    }
                }
                if (e.action == 0)
                    release();
            } break;
            case cuiui::EventType::MouseMotionEvent: {
                if (grab_flag) {
                    if (grabbed_point)
                        drag_item();
                    else
                        drag_view();
                }
            } break;
            case cuiui::EventType::MouseScrollEvent: {
                auto &e = std::get<cuiui::MouseScrollEvent>(event.data);
                zoom -= e.offset.y / 1000 * zoom;
                if (zoom < 0.001f)
                    zoom = 0.001f;
                if (zoom > 48.0f)
                    zoom = 48.0f;
            } break;
            default: break;
            }
            w->events.pop();
        }

        storage.proj = scale(f32mat4::identity(), {aspect, -1.0f, 1.0f});
        storage.view = scale(translate(f32mat4::identity(), {view_pos.x / aspect, view_pos.y, 0.0f}), {zoom, zoom, zoom});
        storage.mouse = {mouse_view.x, mouse_view.y};

        gl_ctx.make_current();
        auto &temp_storage = *reinterpret_cast<Storage *>(glMapNamedBuffer(sbo_id, GL_READ_WRITE));
        temp_storage = storage;
        glUnmapNamedBuffer(sbo_id);
        glViewport(0, 0, w->size.x, w->size.y);
        glUseProgram(shader_program_id);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, sbo_id);
        glBindVertexArray(vao_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
        gl_ctx.swap_buffers();
    }
    glDeleteProgram(shader_program_id);
    glDeleteBuffers(1, &vbo_id);
    glDeleteVertexArrays(1, &vao_id);
}
