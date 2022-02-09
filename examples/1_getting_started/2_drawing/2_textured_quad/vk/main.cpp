#include <cuiui/cuiui.hpp>
#include <cuiui/platform/defaults.hpp>
#include <coel/vulkan/core.hpp>
#include "../0_common/data.hpp"
namespace cuiui_default = cuiui::platform::defaults;

int main() {
    cuiui_default::Context ui;
    coel::vulkan::Instance vk_instance;
    coel::vulkan::PhysicalDevice vk_physical_device = coel::vulkan::choose_physical_device(vk_instance.handle);
    coel::vulkan::Surface vk_surface;
    {
        auto w = ui.window({.id = "w", .size = {400, 400}});
        // w->lock_mouse();
        vk_surface = coel::vulkan::Surface(vk_instance.handle, *w);
    }
    vk_surface.select_format(vk_physical_device);
    uint32_t vk_queue_family_index = coel::vulkan::select_graphics_and_present_queue(vk_physical_device, vk_surface.handle);
    coel::vulkan::Device vk_device(vk_instance.handle, vk_physical_device, {vk_queue_family_index});
    coel::vulkan::Swapchain vk_swapchain(vk_physical_device, vk_surface, vk_device.handle, vk_queue_family_index);
    auto vk_memory_properties = coel::vulkan::get_physical_device_memory_properties(vk_physical_device);
    coel::vulkan::Buffer vbo(vk_device.handle, vk_memory_properties, vertices.data(), sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    coel::vulkan::Buffer ibo(vk_device.handle, vk_memory_properties, indices.data(), sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    coel::vulkan::Buffer ubo(vk_device.handle, vk_memory_properties, &uniforms, sizeof(uniforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    coel::vulkan::Image vk_image(vk_device.handle, vk_memory_properties, static_cast<uint32_t>(image.size_x), static_cast<uint32_t>(image.size_y));
    coel::vulkan::CommandPool vk_command_pool(vk_device.handle, vk_queue_family_index);
    auto vk_cmd = vk_command_pool.get_command_buffer();
    vk_cmd.begin();
    vk_image.upload(vk_cmd.handle, image.pixels, static_cast<uint32_t>(image.channels));
    vk_cmd.end();
    vk_cmd.submit_blocking(vk_device.queues[0]);

    volkInitialize();
    volkLoadInstance(vk_instance.handle);
    volkLoadDevice(vk_device.handle);

    // coel::vulkan::Descriptors vk_descriptors({
    //     .layout{
    //         {
    //             .binding = 0,
    //             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //             .descriptorCount = 1,
    //             .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    //             .pImmutableSamplers = nullptr,
    //         },
    //         {
    //             .binding = 1,
    //             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    //             .descriptorCount = 1,
    //             .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    //             .pImmutableSamplers = nullptr,
    //         },
    //     },
    //     .pool_sizes{
    //         {
    //             .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //             .descriptorCount = 1,
    //         },
    //         {
    //             .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    //             .descriptorCount = static_cast<uint32_t>(vk_swapchain.image_resources.size() * 1),
    //         },
    //     },
    //     .sets{
    //     },
    // });

    VkDescriptorSetLayout vk_desc_layout;
    {
        const VkDescriptorSetLayoutBinding layout_bindings[]{
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr,
            },
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            },
        };
        const VkDescriptorSetLayoutCreateInfo desc_layout_ci = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .bindingCount = 2,
            .pBindings = layout_bindings,
        };
        vkCreateDescriptorSetLayout(vk_device.handle, &desc_layout_ci, nullptr, &vk_desc_layout);
    }

    VkDescriptorPool vk_desc_pool;
    {
        const VkDescriptorPoolSize type_counts[]{
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
            },
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = static_cast<uint32_t>(vk_swapchain.image_resources.size() * 1),
            },
        };
        const VkDescriptorPoolCreateInfo desc_pool_ci = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .maxSets = static_cast<uint32_t>(vk_swapchain.image_resources.size()),
            .poolSizeCount = 2,
            .pPoolSizes = type_counts,
        };
        vkCreateDescriptorPool(vk_device.handle, &desc_pool_ci, nullptr, &vk_desc_pool);
    }

    std::vector<VkDescriptorSet> vk_descriptor_sets;
    vk_descriptor_sets.resize(vk_swapchain.image_resources.size());
    {
        VkDescriptorImageInfo tex_descs[1];
        VkWriteDescriptorSet writes[2];
        VkDescriptorSetAllocateInfo desc_set_alloc_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = vk_desc_pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &vk_desc_layout,
        };
        VkDescriptorBufferInfo buffer_info;
        buffer_info.offset = 0;
        buffer_info.range = sizeof(Uniforms);
        memset(&tex_descs, 0, sizeof(tex_descs));
        for (unsigned int i = 0; i < 1; i++) {
            tex_descs[i].sampler = vk_image.sampler;
            tex_descs[i].imageView = vk_image.view;
            tex_descs[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
        memset(&writes, 0, sizeof(writes));

        writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[0].dstBinding = 0;
        writes[0].descriptorCount = 1;
        writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writes[0].pBufferInfo = &buffer_info;

        writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[1].dstBinding = 1;
        writes[1].descriptorCount = 1;
        writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[1].pImageInfo = tex_descs;

        for (unsigned int i = 0; i < vk_swapchain.image_resources.size(); i++) {
            vkAllocateDescriptorSets(vk_device.handle, &desc_set_alloc_info, &vk_descriptor_sets[i]);
            buffer_info.buffer = ubo.handle;
            writes[0].dstSet = vk_descriptor_sets[i];
            writes[1].dstSet = vk_descriptor_sets[i];
            vkUpdateDescriptorSets(vk_device.handle, 2, writes, 0, nullptr);
        }
    }

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
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = sizeof(float) * 0,
            },
            {
                .location = 1,
                .binding = 0,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = sizeof(float) * 3,
            },
        },
        .descriptor_set_layouts = {vk_desc_layout},
    });

    vk_swapchain.draw_cmd_func = [&](VkCommandBuffer cmd) {
        const VkCommandBufferBeginInfo cmd_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
            .pInheritanceInfo = nullptr,
        };
        vkBeginCommandBuffer(cmd, &cmd_begin_info);
        vk_swapchain.begin_renderpass(cmd, {0.3f, 0.2f, 0.2f, 1.0f});
        vk_graphics_pipeline.bind(cmd);
        vkCmdBindDescriptorSets(
            cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_graphics_pipeline.layout, 0, 1,
            &vk_descriptor_sets[vk_swapchain.current_image_index], 0, nullptr);
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
        ibo.bind_ibo(cmd);
        vkCmdDrawIndexed(cmd, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
        vkCmdEndRenderPass(cmd);
        vkEndCommandBuffer(cmd);
    };

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
        vk_swapchain.present_and_swap(vk_device.queues[0]);

        game_state.update(w);
        Uniforms temp_uniforms{
            .proj = scale(f32mat4::identity(), {static_cast<float>(w->size.y) / w->size.x, 1.0f, 0.01f}),
            // .proj = perspective(radians(45.0f), static_cast<float>(w->size.x) / w->size.y, 0.01f, 100.0f),
            .view = translate(rotate(rotate(uniforms.view, game_state.rot_y, {0, 1, 0}), game_state.rot_x, {1, 0, 0}), {game_state.pos_x, game_state.pos_y, game_state.pos_z}),
        };
        ubo.upload(&temp_uniforms, sizeof(temp_uniforms));

        if (vk_swapchain.was_resized()) {
            for (size_t i = 0; i < vk_swapchain.image_resources.size() - 1; ++i) {
                vk_swapchain.wait_for_frame();
                vk_swapchain.present_and_swap(vk_device.queues[0]);
            }
        }
    }
    vk_device.wait_idle();

    vkDestroyDescriptorSetLayout(vk_device.handle, vk_desc_layout, nullptr);
    vkDestroyDescriptorPool(vk_device.handle, vk_desc_pool, nullptr);
}
