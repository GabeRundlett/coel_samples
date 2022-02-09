
#include "opengl.hpp"
#include "vulkan.hpp"

void handle_events(AppState &state, WindowHandle w) {
    while (!w->events.empty()) {
        auto &event = w->events.back();
        switch (event.type) {
        case cuiui::EventType::MouseMotionEvent: {
            auto &event_data = std::get<cuiui::MouseMotionEvent>(event.data);
            state.mouse_x = static_cast<float>(event_data.pos.x) / static_cast<float>(w->size.x) * 2.0f - 1.0f;
            state.mouse_y = static_cast<float>(event_data.pos.y) / static_cast<float>(w->size.y) * 2.0f - 1.0f;
        } break;
        case cuiui::EventType::MouseButtonEvent: {
            auto &event_data = std::get<cuiui::MouseButtonEvent>(event.data);
            if (event_data.action == 0 && event_data.key == 0)
                if (state.mouse_x > 0.9f - 0.0001f && state.mouse_y < -0.9f)
                    w->should_close = true;
        } break;
        default: break;
        }
        w->events.pop();
    }
    state.size_x = w->size.x;
    state.size_y = w->size.y;
}

int main() {
    AppState state;
    UiContext ui;
    OpenGLApp glapp;
    VulkanApp vkapp;

    {
        auto w = ui.window({.id = "w_gl", .title = "OpenGL Window", .size = {400, 400}});
        glapp.init(state, w);
    }
    {
        auto w = ui.window({.id = "w_vk", .title = "Vulkan Window", .size = {400, 400}});
        vkapp.init(state, w);
    }

    while (true) {
        {
            auto w = ui.window({.id = "w_gl"});
            if (w->should_close)
                break;
            handle_events(state, w);
            glapp.update(state, w);
        }
        {
            auto w = ui.window({.id = "w_vk"});
            if (w->should_close)
                break;
            handle_events(state, w);
            vkapp.update(state, w);
        }
    }

    {
        auto w = ui.window({.id = "w_gl"});
        glapp.deinit(state, w);
    }
    {
        auto w = ui.window({.id = "w_vk"});
        vkapp.deinit(state, w);
    }
}
