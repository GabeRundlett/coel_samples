#pragma once

#include "../render_element.hpp"

namespace cuiui::components {
    struct Quad {
        struct Config {
            f32vec2 pos, size;
            f32vec4 col;
        };
        f32vec2 pos, size;
        f32vec4 col;

        Quad() = default;
        Quad(size_t, const Config &config) {
            pos = config.pos;
            size = config.size;
            col = config.col;
        }

        void submit(auto &, const Config &) {
        }

        void update(auto &ui) {
            ui.handle_events([&](const auto &event) {
                bool consume = false;
                switch (event.index()) {
                case cuiui::EventType::MouseMotionEvent: {
                    consume = in_rect(f32rect{.p0 = pos, .p1 = pos + size}, ui.mouse_pos);
                } break;
                default: break;
                }
                return consume;
            });
        }

        void render(auto &ui) {
            ui.render_elements.push_back(RenderElement{
                .rect = f32rect{
                    .p0 = pos,
                    .p1 = pos + size,
                },
                .col = col,
            });
        }
    };

    struct Circle {
        struct Config {
            f32vec2 pos, size;
            f32vec4 col;
        };
        f32vec2 pos, size;
        f32vec4 col;

        Circle() = default;
        Circle(size_t, const Config &config) {
            pos = config.pos;
            size = config.size;
            col = config.col;
        }

        void submit(auto &, const Config &) {
        }

        void update(auto &ui) {
            ui.handle_events([&](const auto &event) {
                bool consume = false;
                switch (event.index()) {
                case cuiui::EventType::MouseMotionEvent: {
                    consume = in_rect(f32rect{.p0 = pos, .p1 = pos + size}, ui.mouse_pos);
                } break;
                default: break;
                }
                return consume;
            });
        }

        void render(auto &ui) {
            auto rect = f32rect{
                .p0 = pos,
                .p1 = pos + size,
            };
            ui.render_elements.push_back(RenderElement{
                .rect = rect,
                .col = col,
                .props = {
                    2.0f,
                    0.0f,
                    rect.p1[0] - rect.p0[0],
                    rect.p1[1] - rect.p0[1],
                },
            });
        }
    };
} // namespace cuiui::components
