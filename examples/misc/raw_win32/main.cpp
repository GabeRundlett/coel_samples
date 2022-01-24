#include <cuiui/platform/win32.hpp>

struct Window : cuiui::WindowState {
    using WindowHandleType = cuiui::WindowHandle<Window>;
    static constexpr const char *window_class_name = "raw_win32";
    HWND hwnd;
    void create(const cuiui::WindowConfig &config) {
        std::string title_str(config.title);
        hwnd = CreateWindowExA(
            WS_EX_OVERLAPPEDWINDOW, window_class_name, title_str.c_str(), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            static_cast<int32_t>(config.size.x), static_cast<int32_t>(config.size.y),
            nullptr, nullptr, GetModuleHandle(nullptr), this);
        ShowWindow(hwnd, SW_SHOW);
    }
    void destroy() {
        if (hwnd) {
            DestroyWindow(hwnd);
        }
    }
    void update() {
        MSG msg;
        while (PeekMessageW(&msg, hwnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    static LRESULT wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        return DefWindowProcA(hwnd, msg, wp, lp);
    };
    static void register_wc() {
        WNDCLASSEXA window_class{
            .cbSize = sizeof(WNDCLASSEXA),
            .style = 0,
            .lpfnWndProc = wnd_proc,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = GetModuleHandle(nullptr),
            .hIcon = LoadIcon(NULL, IDI_APPLICATION),
            .hCursor = LoadCursor(NULL, IDC_ARROW),
            .hbrBackground = nullptr,
            .lpszMenuName = nullptr,
            .lpszClassName = window_class_name,
            .hIconSm = LoadIcon(NULL, IDI_WINLOGO),
        };
        RegisterClassExA(&window_class);
    }
    static void unregister_wc() {
        UnregisterClassA(window_class_name, GetModuleHandleA(nullptr));
    }
};

int main() {
    using UiContext = cuiui::platform::win32::Context<Window>;
    UiContext ui;
    ui.window({.id = "w", .title = "Raw Win32 Window", .size_x = 400, .size_y = 400});

    while (true) {
        auto w = ui.window({.id = "w"});
        if (w->should_close)
            break;

        ui.update();
    }
}
