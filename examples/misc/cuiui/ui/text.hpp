#pragma once

#include "../render_element.hpp"
#include <string>

namespace cuiui::components {
    struct Text {
        static inline float size_x = 100.0f;
        static inline float size_y = 16.0f;

        std::string content;
        f32rect rect{};

        struct Config {
            std::string content;
        };

        Text() = default;
        Text(const Text &) = delete;
        Text(Text &&) = default;
        Text &operator=(const Text &) = delete;
        Text &operator=(Text &&) = default;
        Text(size_t, const Config &config) {
            content = config.content;
        }

        void submit(auto &ui, const Config &config) {
            content = config.content;
            rect = f32rect{
                .p0 = {0.0f + ui.pt[0], 0.0f + ui.pt[1]},
                .p1 = {size_x + ui.pt[0], size_y + ui.pt[1]},
            };
            ui.pt[1] += size_y;
            ui.add_margin();
        }

        void update(auto &) {
        }

        void render(auto &ui) {
            draw_text(ui, rect, content);
        }
    };
} // namespace cuiui::components
