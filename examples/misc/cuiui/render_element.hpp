#pragma once

#include <cuiui/math/types.hpp>
#include <string>
#include <string_view>

namespace cuiui {
    struct RenderElement {
        f32rect rect;
        f32vec4 col;
        f32vec4 props;

        std::string_view content;
    };
} // namespace cuiui

constexpr bool in_rect(f32rect rect, f32vec2 p) {
    return p[0] >= rect.p0[0] && p[0] < rect.p1[0] &&
           p[1] >= rect.p0[1] && p[1] < rect.p1[1];
}

namespace cuiui::components {
    void draw_rect(auto &ui, const auto &rect, const auto &col) {
        ui.render_elements.push_back(RenderElement{.rect = rect, .col = col});
    }
    void draw_rounded_rect(auto &ui, const auto &rect, const auto &col) {
        f32 max_d = std::min(std::abs(rect.p0[0] - rect.p1[0]), std::abs(rect.p0[1] - rect.p1[1]));
        f32 d = std::min(10.0f, max_d), r = d * 0.5f;

        auto r0 = rect;
        r0.p0[0] += r, r0.p1[0] -= r;
        auto r1 = rect;
        r1.p0[1] += r, r1.p1[1] -= r;

        ui.render_elements.push_back(RenderElement{.rect = r0, .col = col});
        ui.render_elements.push_back(RenderElement{.rect = r1, .col = col});

        f32rect crect;
        f32vec4 cprops = {2.0f, 0.0f, d, d};

        crect = f32rect{
            .p0 = rect.p0,
            .p1 = rect.p0 + f32vec2{d, d},
        };
        ui.render_elements.push_back(RenderElement{.rect = crect, .col = col, .props = cprops});
        crect = f32rect{
            .p0 = rect.p1 - f32vec2{d, d},
            .p1 = rect.p1,
        };
        ui.render_elements.push_back(RenderElement{.rect = crect, .col = col, .props = cprops});
        crect = f32rect{
            .p0 = f32vec2{rect.p0[0], rect.p1[1]} - f32vec2{0.0f, d},
            .p1 = f32vec2{rect.p0[0], rect.p1[1]} + f32vec2{d, 0.0f},
        };
        ui.render_elements.push_back(RenderElement{.rect = crect, .col = col, .props = cprops});
        crect = f32rect{
            .p0 = f32vec2{rect.p1[0], rect.p0[1]} - f32vec2{d, 0.0f},
            .p1 = f32vec2{rect.p1[0], rect.p0[1]} + f32vec2{0.0f, d},
        };
        ui.render_elements.push_back(RenderElement{.rect = crect, .col = col, .props = cprops});
    }
    void draw_background(auto &ui, const auto &rect, bool highlighted) {
        auto col = highlighted ? ui.colors.elem_background_highlight : ui.colors.elem_background;
        draw_rect(ui, rect, col);
    }
    void draw_foreground(auto &ui, const auto &rect) {
        draw_rect(ui, rect, ui.colors.elem_foreground);
    }
    void draw_outline(auto &ui, const auto &rect) {
        auto outline_rect = rect;
        outline_rect.p0 -= f32vec2{10.0f, 10.0f};
        outline_rect.p1 += f32vec2{10.0f, 10.0f};
        ui.render_elements.push_back(RenderElement{
            .rect = outline_rect,
            .col = ui.colors.elem_outline,
            .props = {
                1.0f,
                10.0f,
                outline_rect.p1[0] - outline_rect.p0[0],
                outline_rect.p1[1] - outline_rect.p0[1],
            },
        });
    }
    void draw_text(auto &ui, const auto &rect, const std::string &content) {
        auto props = f32vec4{4.0f, 0.0f, 0.0f, 0.0f};
        ui.render_elements.push_back(RenderElement{.rect = rect, .col = ui.colors.text, .props = props, .content = content});
    }
} // namespace cuiui::components
