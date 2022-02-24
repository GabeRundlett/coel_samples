#pragma once

#include "../render_element.hpp"

namespace cuiui::components {
    struct ColorEditRGB {
        static inline float size_x = 200.0f;
        static inline float size_y = 200.0f;
        static inline float hue_sx = 10.0f;
        static inline float grab_bar_width = 4.0f;
        static inline float hue_grab_bar_width = 1.0f;

        f32rect rect;
        f32vec2 value = {0.0f, 0.0f};
        bool grabbed = false;
        f32vec2 grab_offset = {0.0f, 0.0f};
        bool hovered = false;

        f32rect hue_rect;
        float hue_value = 0.0f;
        float hue_grab_offset = 0.0f;
        bool hue_grabbed = false;
        bool hue_hovered = false;

        f32vec3 rgb;

        struct Config {
        };

        ColorEditRGB() = default;
        ColorEditRGB(size_t, const Config &) {
        }

        void submit(auto &ui, const Config &) {
            rect = f32rect{
                .p0 = {0.0f + ui.pt[0], 0.0f + ui.pt[1]},
                .p1 = {size_x + ui.pt[0], size_y + ui.pt[1]},
            };
            hue_rect = rect;
            hue_rect.p0[0] += size_x, hue_rect.p1[0] = hue_rect.p0[0] + hue_sx;
            ui.pt[1] += size_y;
            ui.add_margin();
        }

        void update(auto &ui) {
            ui.handle_events([&](const auto &event) {
                bool consume = false;
                switch (event.index()) {
                case cuiui::EventType::MouseMotionEvent: {
                    auto &e = std::get<cuiui::MouseMotionEvent>(event);
                    hovered = in_rect(rect, e.pos);
                    hue_hovered = in_rect(hue_rect, e.pos);
                    consume = hovered;
                } break;
                case cuiui::EventType::MouseButtonEvent: {
                    auto &e = std::get<cuiui::MouseButtonEvent>(event);
                    if (e.action == 1) {
                        switch (e.key) {
                        case 0: {
                            if (hovered) {
                                grab_offset = ui.mouse_pos - rect.p0;
                                grabbed = true, consume = true;
                            }
                            if (hue_hovered) {
                                hue_grab_offset = ui.mouse_pos[1];
                                hue_grabbed = true, consume = true;
                            }
                        } break;
                        }
                    }
                    if (e.action == 0)
                        grabbed = false, hue_grabbed = false;
                } break;
                default: break;
                }
                return consume;
            });

            if (grabbed) {
                value = ui.mouse_pos;

                if (value[0] < rect.p0[0] + grab_bar_width)
                    value[0] = rect.p0[0] + grab_bar_width;
                if (value[0] > rect.p0[0] - grab_bar_width + size_x)
                    value[0] = rect.p0[0] - grab_bar_width + size_x;

                if (value[1] < rect.p0[1] + grab_bar_width)
                    value[1] = rect.p0[1] + grab_bar_width;
                if (value[1] > rect.p0[1] - grab_bar_width + size_y)
                    value[1] = rect.p0[1] - grab_bar_width + size_y;

                value = value - rect.p0 - f32vec2{grab_bar_width, grab_bar_width};
                value[0] /= size_x - grab_bar_width * 2.0f;
                value[1] /= size_y - grab_bar_width * 2.0f;
            }

            if (hue_grabbed) {
                hue_value = ui.mouse_pos[1];
                if (hue_value < rect.p0[1] + hue_grab_bar_width)
                    hue_value = rect.p0[1] + hue_grab_bar_width;
                if (hue_value > rect.p0[1] - hue_grab_bar_width + size_y)
                    hue_value = rect.p0[1] - hue_grab_bar_width + size_y;
                hue_value = (hue_value - rect.p0[1] - hue_grab_bar_width) / (size_y - hue_grab_bar_width * 2.0f);
            }

            auto hsv2rgb = [](auto c) {
                f32 k0 = 3.0f / 3.0f;
                f32 k1 = 2.0f / 3.0f;
                f32 k2 = 1.0f / 3.0f;

                f32 p0 = std::fabs(std::fmodf(c[0] + k0, 1.0f) * 6.0f - 3.0f);
                f32 p1 = std::fabs(std::fmodf(c[0] + k1, 1.0f) * 6.0f - 3.0f);
                f32 p2 = std::fabs(std::fmodf(c[0] + k2, 1.0f) * 6.0f - 3.0f);

                f32 fac = 1.0f - c[1];

                return f32vec3{
                    c[2] * (k0 * fac + std::max(std::min(p0 - k0, 1.0f), 0.0f) * (1.0f - fac)),
                    c[2] * (k0 * fac + std::max(std::min(p1 - k0, 1.0f), 0.0f) * (1.0f - fac)),
                    c[2] * (k0 * fac + std::max(std::min(p2 - k0, 1.0f), 0.0f) * (1.0f - fac)),
                };
            };
            rgb = hsv2rgb(f32vec3{hue_value, 1.0f - value[0], 1.0f - value[1]});
        }

        void render(auto &ui) {
            auto outline_rect = rect;
            outline_rect.p1[0] += hue_sx;
            draw_outline(ui, outline_rect);
            draw_background(ui, rect, hovered || grabbed);
            auto inside_rect = rect;
            inside_rect.p0 += f32vec2{grab_bar_width, grab_bar_width};
            inside_rect.p1 -= f32vec2{grab_bar_width, grab_bar_width};
            ui.render_elements.push_back(RenderElement{
                .rect = inside_rect,
                .col = (hovered || grabbed) ? ui.colors.elem_background_highlight : ui.colors.elem_background,
                .props = {
                    3.0f,
                    hue_value,
                },
            });
            const auto rx = rect.p0[0] + value[0] * (size_x - grab_bar_width * 2.0f) + grab_bar_width;
            const auto ry = rect.p0[1] + value[1] * (size_y - grab_bar_width * 2.0f) + grab_bar_width;
            auto grab_rect = f32rect{
                .p0 = {rx - grab_bar_width, ry - grab_bar_width},
                .p1 = {rx + grab_bar_width, ry + grab_bar_width},
            };
            ui.render_elements.push_back(RenderElement{
                .rect = grab_rect,
                .col = ui.colors.elem_background,
                .props = {
                    2.0f,
                    0.0f,
                    grab_rect.p1[0] - grab_rect.p0[0],
                    grab_rect.p1[1] - grab_rect.p0[1],
                },
            });
            grab_rect.p0 += f32vec2{1.0f, 1.0f};
            grab_rect.p1 -= f32vec2{1.0f, 1.0f};
            ui.render_elements.push_back(RenderElement{
                .rect = grab_rect,
                .col = ui.colors.elem_foreground,
                .props = {
                    2.0f,
                    0.0f,
                    grab_rect.p1[0] - grab_rect.p0[0],
                    grab_rect.p1[1] - grab_rect.p0[1],
                },
            });

            draw_background(ui, hue_rect, hue_hovered || hue_grabbed);
            const auto hue_ry = rect.p0[1] + hue_value * (size_y - hue_grab_bar_width * 2.0f) + hue_grab_bar_width;
            auto hue_grab_rect = f32rect{
                .p0 = {hue_rect.p0[0], hue_ry - hue_grab_bar_width},
                .p1 = {hue_rect.p1[0], hue_ry + hue_grab_bar_width},
            };
            draw_foreground(ui, hue_grab_rect);
        }
    };
} // namespace cuiui::components
