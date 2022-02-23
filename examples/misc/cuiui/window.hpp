#pragma once

#include <variant>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "events.hpp"
#include "render_element.hpp"

struct Window {
    struct GlfwState {
        GLFWwindow *window_handle;
    };
    std::variant<GlfwState, std::monostate> platform_state;
    struct Config {
        f32vec2 pos, dim;
    };

    f32vec2 pos, dim;
    bool should_close;

    bool grabbed = false;
    f32vec2 grab_offset = {0.0f, 0.0f};

    std::vector<cuiui::Event> events;

    Window() = default;
    Window(size_t depth, const Config &config) {
        if (depth == 0) {
            pos = config.pos;
            dim = config.dim;
            should_close = false;
            platform_state = GlfwState{};
            auto &glfw_state = std::get<GlfwState>(platform_state);
            glfwInit();
            glfw_state.window_handle = glfwCreateWindow(static_cast<i32>(dim[0]), static_cast<i32>(dim[1]), "title", nullptr, nullptr);
            glfwMakeContextCurrent(glfw_state.window_handle);
            // glfwSetWindowPos(glfw_state.window_handle, pos[0], pos[1]);
            int x, y;
            glfwGetWindowPos(glfw_state.window_handle, &x, &y);
            pos[0] = static_cast<f32>(x), pos[1] = static_cast<f32>(y);
            gladLoadGL();
            glfwSetWindowUserPointer(glfw_state.window_handle, this);
            glfwSetWindowCloseCallback(glfw_state.window_handle, [](GLFWwindow *glfw_window) {
                auto *window_ptr = reinterpret_cast<::Window *>(glfwGetWindowUserPointer(glfw_window));
                if (!window_ptr)
                    return;
                window_ptr->should_close = true;
            });
            glfwSetWindowSizeCallback(glfw_state.window_handle, [](GLFWwindow *glfw_window, int x, int y) {
                auto *window_ptr = reinterpret_cast<::Window *>(glfwGetWindowUserPointer(glfw_window));
                if (!window_ptr)
                    return;
                window_ptr->dim = {static_cast<f32>(x), static_cast<f32>(y)};
            });
            glfwSetCursorPosCallback(glfw_state.window_handle, [](GLFWwindow *glfw_window, double x, double y) {
                auto *window_ptr = reinterpret_cast<::Window *>(glfwGetWindowUserPointer(glfw_window));
                if (!window_ptr)
                    return;
                window_ptr->events.push_back(cuiui::MouseMotionEvent{.pos{static_cast<f32>(x), static_cast<f32>(y)}});
            });
            glfwSetMouseButtonCallback(glfw_state.window_handle, [](GLFWwindow *glfw_window, i32 button, i32 action, i32 ) {
                auto *window_ptr = reinterpret_cast<::Window *>(glfwGetWindowUserPointer(glfw_window));
                if (!window_ptr)
                    return;
                window_ptr->events.push_back(cuiui::MouseButtonEvent{.key = static_cast<u32>(button), .action = static_cast<u32>(action)});
            });
        } else {
            platform_state = std::monostate{};
            pos = config.pos;
            dim = config.dim;
        }
    }
    Window(const Window &other) = delete;
    Window(Window &&other) {
        *this = std::move(other);
    }
    Window &operator=(const Window &other) = delete;
    Window &operator=(Window &&other) {
        pos = other.pos;
        dim = other.dim;
        should_close = other.should_close;
        platform_state = other.platform_state;
        switch (platform_state.index()) {
        case 0: {
            auto &glfw_state = std::get<GlfwState>(platform_state);
            glfwSetWindowUserPointer(glfw_state.window_handle, this);
        } break;
        default: break;
        }
        return *this;
    }

    void submit(auto &ui) {
        glfwPollEvents();
        switch (platform_state.index()) {
        case 0: {
            ui.events.insert(ui.events.end(), events.begin(), events.end());
            events.clear();
        } break;
        case 1: {
            ui.pt = pos + ui.pt_margin;
        } break;
        default: break;
        }
    }
    void update(auto &ui) {
        switch (platform_state.index()) {
        case 1: {
            auto rect = f32rect{
                .p0 = pos,
                .p1 = pos + dim,
            };
            ui.handle_events([&](const auto &event) {
                bool consume = false;
                switch (event.index()) {
                case cuiui::EventType::MouseMotionEvent: {
                    consume = in_rect(rect, ui.mouse_pos);
                } break;
                case cuiui::EventType::MouseButtonEvent: {
                    auto &e = std::get<cuiui::MouseButtonEvent>(event);
                    if (e.action == 1) {
                        switch (e.key) {
                        case 0: {
                            if (in_rect(rect, ui.mouse_pos)) {
                                grab_offset = ui.mouse_pos - pos;
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
            if (grabbed)
                pos = ui.mouse_pos - grab_offset;
        } break;
        default: break;
        }
    }
    void render(auto &ui) {
        switch (platform_state.index()) {
        case 0: {
            auto &glfw_state = std::get<GlfwState>(platform_state);
            glfwMakeContextCurrent(glfw_state.window_handle);
            glfwSwapBuffers(glfw_state.window_handle);
        } break;
        case 1: {
            auto rect = f32rect{
                .p0 = pos,
                .p1 = pos + dim,
            };
            cuiui::components::draw_outline(ui, rect);
            ui.render_elements.push_back(cuiui::RenderElement{
                .rect = rect,
                .col = ui.colors.background,
            });
        } break;
        default: break;
        }
    }
};
