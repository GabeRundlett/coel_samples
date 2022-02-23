#pragma once

#include <ranges>
#include <algorithm>
#include <vector>
#include <string_view>

#include <iostream>

#include "events.hpp"
#include "render_element.hpp"

#include "ui/button.hpp"
#include "ui/checkbox.hpp"
#include "ui/coloredit.hpp"
#include "ui/slider.hpp"
#include "ui/shapes.hpp"

namespace cuiui {
    template <typename T>
    struct ComponentConfig {
        std::string_view id;
        typename T::Config init;
    };

    template <typename... BaseTypes>
    struct Component {
        std::string_view id;
        std::variant<BaseTypes...> base;
        bool is_stale;
    };

    template <typename T>
    struct ComponentIndex {
        size_t i;
    };

    struct UiColors {
        static constexpr f32 scl = 5.0f;
        // clang-format off
        f32vec4 foreground                = {1.00f * scl, 0.00f * scl, 1.00f * scl, 1.0f};
        f32vec4 background                = {0.10f * scl, 0.10f * scl, 0.10f * scl, 1.0f};
        f32vec4 elem_foreground           = {0.24f * scl, 0.24f * scl, 0.24f * scl, 1.0f};
        f32vec4 elem_background           = {0.12f * scl, 0.12f * scl, 0.12f * scl, 1.0f};
        f32vec4 elem_background_highlight = {0.15f * scl, 0.15f * scl, 0.15f * scl, 1.0f};
        f32vec4 elem_outline              = {0.00f * scl, 0.00f * scl, 0.00f * scl, 1.0f};
        // clang-format on
    };

    template <typename... Components>
    struct BasicContext {
        using ComponentType = Component<Components...>;

        std::vector<Event> events;
        std::vector<RenderElement> render_elements;
        std::vector<ComponentType> components;
        std::vector<size_t> component_indices;

        UiColors colors;
        f32vec2 mouse_pos;
        size_t depth;

        f32vec2 pt;
        static inline f32vec2 pt_margin = {6.0f, 6.0f};
        void add_margin() {
            pt[1] += pt_margin[1];
        }

        void new_frame() {
            component_indices.clear();
            render_elements.clear();
            events.clear();
            pt = f32vec2{0.0f, 0.0f} + pt_margin;
            depth = 0;
            for (auto &c : components)
                c.is_stale = true;
        }

        template <typename T>
        ComponentIndex<T> submit(const ComponentConfig<T> &config) {
            auto find_component = std::ranges::find_if(components, [&](const auto &c) { return c.id == config.id; });
            size_t component_i;
            if (find_component != components.end()) {
                component_i = find_component - components.begin();
            } else {
                component_i = components.size();
                components.push_back(Component<Components...>{.id = config.id, .base = T(depth, config.init), .is_stale = true});
            }
            auto &c = components[component_i];
            component_indices.push_back(component_i);
            auto &c_base = std::get<T>(c.base);
            c_base.submit(*this);
            c.is_stale = false;
            return {.i = component_i};
        }

        template <typename T>
        T &get(std::string_view id) {
            auto find_component = std::ranges::find_if(components, [&](const auto &c) { return c.id == id; });
            return std::get<T>(find_component->base);
        }

        template <typename T>
        T &get(ComponentIndex<T> index) {
            return std::get<T>(components[index.i].base);
        }

        void render_frame() {
            auto &ui = *this;
            for (size_t i : component_indices)
                std::visit([&](auto &&c) { c.render(ui); }, components[i].base);
            handle_events([&](const auto &event) {
                switch (event.index()) {
                case cuiui::EventType::MouseMotionEvent: {
                    auto &e = std::get<cuiui::MouseMotionEvent>(event);
                    ui.mouse_pos = e.pos;
                } break;
                }
                return false;
            });
            std::ranges::for_each(component_indices | std::views::reverse, [&](size_t i) {
                auto &c = components[i];
                std::visit([&](auto &&c_base) { c_base.update(ui); }, c.base);
            });
            std::erase_if(components, [](const auto &c) { return c.is_stale; });
        }

        void handle_events(auto &&pred) {
            auto [first, last] = std::ranges::remove_if(events, pred);
            events.erase(first, last);
        }

        struct Scope {
            BasicContext *ui_ctx;
            ~Scope() {
                --ui_ctx->depth;
            }
        };

        Scope scope() {
            ++depth;
            return {.ui_ctx = this};
        }
    };

    using Context = BasicContext<
        components::Button,
        components::Checkbox,
        components::Slider,
        components::ColorEditRGB,

        components::Quad,
        components::Circle,

        Window>;
} // namespace cuiui
