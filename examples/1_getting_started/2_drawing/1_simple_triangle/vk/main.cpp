#include <cuiui/cuiui.hpp>
#include <cuiui/platform/defaults.hpp>
#include <coel/vulkan/core.hpp>
#include <thread>
#include "../0_common.hpp"
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
    coel::vulkan::Swapchain vk_swapchain(vk_physical_device, vk_surface, vk_device.handle, vk_queue_family_index);
    coel::vulkan::GraphicsPipeline vk_graphics_pipeline({
        .device_handle = vk_device.handle,
        .render_pass = vk_swapchain.render_pass,
        .vert_src = vert_src,
        .frag_src = frag_src,
        .bindings = {{
            .binding = 0,
            .stride = sizeof(float) * 5,
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        }},
        .attribs = {
            {
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = sizeof(float) * 0,
            },
            {
                .location = 1,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = sizeof(float) * 2,
            },
        },
    });
    auto vk_memory_properties = coel::vulkan::get_physical_device_memory_properties(vk_physical_device);
    coel::vulkan::Buffer vbo(vk_device.handle, vk_memory_properties, vertices.data(), sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
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
            vk_swapchain.begin_renderpass(cmd, {0.3f, 0.2f, 0.2f, 1.0f});
            vk_graphics_pipeline.bind(cmd);
            VkViewport viewport{
                .x = 0.0f,
                .y = 0.0f,
                .width = static_cast<float>(vk_swapchain.size_x),
                .height = static_cast<float>(vk_swapchain.size_y),
                .minDepth = 0.0f,
                .maxDepth = 1.0f,
            };
            vkCmdSetViewport(cmd, 0, 1, &viewport);
            VkRect2D scissor{
                .offset = {0, 0},
                .extent = {vk_swapchain.size_x, vk_swapchain.size_y},
            };
            vkCmdSetScissor(cmd, 0, 1, &scissor);
            vbo.bind_vbo(cmd);
            vkCmdDraw(cmd, 3, 1, 0, 0);
            vkCmdEndRenderPass(cmd);
            vkEndCommandBuffer(cmd);
            current_image_resources.cmd_recorded = true;
        }
        vk_swapchain.present_and_swap(vk_device.queues[0]);
    }
    vk_device.wait_idle();
}
