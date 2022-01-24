#include <cuiui/cuiui.hpp>
#include <cuiui/platform/defaults.hpp>
#include <coel/vulkan/core.hpp>
#include <iostream>
#include <thread>
namespace cuiui_default = cuiui::platform::defaults;

int main() {
    cuiui_default::Context ui;
    coel::vulkan::Instance vk_instance;
    coel::vulkan::PhysicalDevice vk_physical_device = coel::vulkan::choose_physical_device(vk_instance.handle);
    coel::vulkan::Surface vk_surface;
    {
        auto w = ui.window({.id = "w", .size = {400, 400}});
        vk_surface = coel::vulkan::Surface(vk_instance.handle, *w);
    }
    vk_surface.select_format(vk_physical_device);
    uint32_t vk_queue_family_index = coel::vulkan::select_graphics_and_present_queue(vk_physical_device, vk_surface.handle);
    coel::vulkan::Device vk_device(vk_instance.handle, vk_physical_device, {vk_queue_family_index});
    coel::vulkan::Swapchain vk_swapchain(vk_physical_device, vk_surface.handle, vk_surface.format, vk_device.handle, vk_queue_family_index);
    volkInitialize();
    volkLoadInstance(vk_instance.handle);
    volkLoadDevice(vk_device.handle);
    while (true) {
        auto w = ui.window({.id = "w"});
        if (w->should_close)
            break;
        if (w->size.x == 0 || w->size.y == 0) {
            using namespace std::literals;
            std::this_thread::sleep_for(100ms);
            continue;
        }
        vk_swapchain.wait_for_frame();
        auto &current_image_resources = vk_swapchain.image_resources[vk_swapchain.current_image_index];
        if (!current_image_resources.cmd_recorded) {
            auto &cmd = current_image_resources.cmd;
            const VkCommandBufferBeginInfo cmd_begin_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
                .pInheritanceInfo = nullptr,
            };
            vkBeginCommandBuffer(cmd, &cmd_begin_info);
            vk_swapchain.begin_renderpass(cmd, {1.0f, 0.0f, 1.0f, 1.0f});
            vkCmdEndRenderPass(cmd);
            vkEndCommandBuffer(cmd);
            current_image_resources.cmd_recorded = true;
        }
        vk_swapchain.present_and_swap(vk_device.queues[0]);
    }
    vk_device.wait_idle();
}
