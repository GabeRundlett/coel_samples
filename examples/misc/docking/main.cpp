#include <cuiui/cuiui.hpp>
#include <cuiui/platform/defaults.hpp>
#include <coel/opengl/core.hpp>
#include <1_getting_started/2_drawing/0_common/scenes/all.hpp>
namespace cuiui_default = cuiui::platform::defaults;

int main() {
    cuiui_default::Context ui;
    RenderContext renderer;
    {
        auto w = ui.window({.id = "w", .size = {400, 400}});
        renderer.attach_to(*w);
    }
    auto blit_pass = BlitWindowPass();
    auto scene = SpinningCubeScene();

    while (true) {
        auto w = ui.window({.id = "w"});
        if (w->should_close)
            break;
        
        blit_pass.begin(w);
        scene.draw();

        renderer.flush();
    }
}
