#pragma once

#include "../render_element.hpp"

namespace cuiui::components {
    struct Checkbox {
        static inline float size_x = 20.0f;
        static inline float size_y = 20.0f;

        f32rect rect{};
        bool grabbed = false;
        bool checked = false;
        bool hovered = false;

        struct Config {
        };

        Checkbox() = default;
        Checkbox(const Checkbox &) = delete;
        Checkbox(Checkbox &&) = default;
        Checkbox &operator=(const Checkbox &) = delete;
        Checkbox &operator=(Checkbox &&) = default;
        Checkbox(size_t, const Config &) {
            grabbed = false;
            checked = false;
            hovered = false;
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
                                grabbed = true, consume = true;
                                checked = !checked;
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
        }

        void render(auto &ui) {
            draw_outline(ui, rect);
            draw_background(ui, rect, hovered);
            if (checked) {
                auto inside_rect = rect;
                inside_rect.p0 += f32vec2{2.0f, 2.0f};
                inside_rect.p1 -= f32vec2{2.0f, 2.0f};
                draw_foreground(ui, inside_rect);
            }
        }
    };
} // namespace cuiui::components
