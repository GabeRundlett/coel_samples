#pragma once

#include <cuiui/math/types.hpp>

namespace cuiui {
    struct RenderElement {
        f32rect rect;
        f32vec4 col;
        f32vec4 props;
    };
} // namespace cuiui

constexpr bool in_rect(f32rect rect, f32vec2 p) {
    return p[0] >= rect.p0[0] && p[0] < rect.p1[0] &&
           p[1] >= rect.p0[1] && p[1] < rect.p1[1];
}

namespace cuiui::components {
    void draw_outline(auto &ui, const auto &rect) {
        auto outline_rect = rect;
        outline_rect.p0 -= f32vec2{8.0f, 8.0f};
        outline_rect.p1 += f32vec2{8.0f, 8.0f};
        ui.render_elements.push_back(RenderElement{
            .rect = outline_rect,
            .col = ui.colors.elem_outline,
            .props = {
                1.0f,
                8.0f,
                rect.p1[0] - rect.p0[0],
                rect.p1[1] - rect.p0[1],
            },
        });
    }
} // namespace cuiui::components
