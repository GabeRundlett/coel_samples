#pragma once

#include "../render_element.hpp"

namespace cuiui::components {
    struct Slider {
        static inline float size_x = 200.0f;
        static inline float size_y = 20.0f;

        f32rect rect;
        float value = 0.0f;
        bool grabbed = false;
        float grab_offset = 0.0f;
        bool hovered = false;

        struct Config {
            float value = 0.0f;
        };

        Slider() = default;
        Slider(size_t, const Config &config) {
            value = config.value;
        }

        void submit(auto &ui) {
            rect = f32rect{
                .p0 = {0.0f + ui.pt[0], 0.0f + ui.pt[1]},
                .p1 = {size_x + ui.pt[0], size_y + ui.pt[1]},
            };
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
                    consume = hovered;
                } break;
                case cuiui::EventType::MouseButtonEvent: {
                    auto &e = std::get<cuiui::MouseButtonEvent>(event);
                    if (e.action == 1) {
                        switch (e.key) {
                        case 0: {
                            if (hovered) {
                                grab_offset = ui.mouse_pos[0];
                                grabbed = true, consume = true;
                            }
                        } break;
                        }
                    }
                    if (e.action == 0)
                        grabbed = false;
                } break;
                default: break;
                }
                return consume;
            });
            if (grabbed) {
                value = ui.mouse_pos[0];
                if (value < rect.p0[0])
                    value = rect.p0[0];
                if (value > rect.p0[0] + size_x)
                    value = rect.p0[0] + size_x;
                value = (value - rect.p0[0]) / size_x;
            }
        }

        void render(auto &ui) {
            draw_outline(ui, rect);
            ui.render_elements.push_back(RenderElement{
                .rect = rect,
                .col = (hovered || grabbed) ? ui.colors.elem_background_highlight : ui.colors.elem_background,
            });
            const auto grab_bar_width = 2.0f;
            const auto rx = rect.p0[0] + value * (size_x - grab_bar_width * 2.0f) + grab_bar_width;
            auto grab_rect = f32rect{
                .p0 = {rx - grab_bar_width, 0.0f + rect.p0[1]},
                .p1 = {rx + grab_bar_width, 0.0f + size_y + rect.p0[1]},
            };
            // draw_outline(ui, grab_rect);
            ui.render_elements.push_back(RenderElement{
                .rect = grab_rect,
                .col = ui.colors.elem_foreground,
            });
        }
    };
} // namespace cuiui::components
