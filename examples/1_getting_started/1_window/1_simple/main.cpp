#include <cuiui/cuiui.hpp>
#include <cuiui/platform/defaults.hpp>

namespace cuiui_default = cuiui::platform::defaults;

int main() {
    cuiui_default::Context ui;
    ui.window({.id = "w", .size = {400, 400}});
    while (true) {
        auto w = ui.window({.id = "w"});
        if (w->should_close)
            break;
    }
}
