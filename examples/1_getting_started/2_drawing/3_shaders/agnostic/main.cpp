#include <cuiui/cuiui.hpp>
#include <cuiui/platform/defaults.hpp>
#include <coel/opengl/core.hpp>
#include "../0_common.hpp"
namespace cuiui_default = cuiui::platform::defaults;

struct RenderContext {
    coel::opengl::Context gl_ctx;

    void attach_to(auto &window) {
        gl_ctx.attach(window);
        gl_ctx.make_current();
        gladLoadGL();
    }

    void flush() {
        gl_ctx.swap_buffers();
    }
};

int main() {
    cuiui_default::Context ui;
    RenderContext renderer;
    {
        auto w = ui.window({.id = "w", .size = {400, 400}});
        renderer.attach_to(*w);
    }
    auto blit_pass = BlitWindowPass();
    auto scene = GonzaScene();

    while (true) {
        auto w = ui.window({.id = "w"});
        if (w->should_close)
            break;
        
        blit_pass.begin(w);
        scene.draw();

        renderer.flush();
    }
}
