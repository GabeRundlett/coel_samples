#include <cuiui/cuiui.hpp>
#include <cuiui/platform/defaults.hpp>
#include <coel/opengl/core.hpp>
#include <glad/glad.h>
namespace cuiui_default = cuiui::platform::defaults;
int main() {
    cuiui_default::Context ui;
    coel::opengl::Context gl_ctx;
    {
        auto w = ui.window({.id = "w", .size = {400, 400}});
        gl_ctx.attach(w->hwnd);
        gl_ctx.make_current();
        gladLoadGL();
    }
    while (true) {
        auto w = ui.window({.id = "w"});
        if (w->should_close)
            break;
        glViewport(0, 0, w->size.x, w->size.y);
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        gl_ctx.swap_buffers();
    }
}
