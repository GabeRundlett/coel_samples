#include "../0_common/scenes/all.hpp"

#include <cuiui/cuiui.hpp>
#include <cuiui/platform/defaults.hpp>
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
        scene.aspect = static_cast<f32>(w->size.x) / static_cast<f32>(w->size.y);
        scene.draw();

        renderer.flush();
    }
}
