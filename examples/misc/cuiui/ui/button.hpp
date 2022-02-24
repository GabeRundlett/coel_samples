#pragma once

#include "../render_element.hpp"

namespace cuiui::components {
    struct Button {
        static inline float size_x = 80.0f;
        static inline float size_y = 40.0f;

        f32rect rect;
        bool grabbed = false;
        bool pressed = false;
        bool hovered = false;

        struct Config {
        };

        Button() = default;
        Button(size_t, const Config &) {
        }

        void submit(auto &ui, const Config &) {
            rect = f32rect{
                .p0 = {0.0f + ui.pt[0], 0.0f + ui.pt[1]},
                .p1 = {size_x + ui.pt[0], size_y + ui.pt[1]},
            };
            ui.pt[1] += size_y;
            ui.add_margin();
        }

        void update(auto &ui) {
            pressed = false;
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
                                pressed = true;
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
            draw_background(ui, rect, hovered || grabbed);
        }
    };
} // namespace cuiui::components
