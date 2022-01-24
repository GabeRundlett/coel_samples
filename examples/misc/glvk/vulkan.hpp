#pragma once

#define VK_NO_PROTOTYPES
#include <coel/vulkan/core.hpp>
#define VOLK_IMPLEMENTATION
#include <volk.h>

#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include "app_common.hpp"

#include <thread>

constexpr TBuiltInResource DEFAULT_SHADER_RESOURCE_SIZES = TBuiltInResource{
    .maxLights = 32,
    .maxClipPlanes = 6,
    .maxTextureUnits = 32,
    .maxTextureCoords = 32,
    .maxVertexAttribs = 64,
    .maxVertexUniformComponents = 4096,
    .maxVaryingFloats = 64,
    .maxVertexTextureImageUnits = 1 << 16,
    .maxCombinedTextureImageUnits = 1 << 16,
    .maxTextureImageUnits = 1 << 16,
    .maxFragmentUniformComponents = 4096,
    .maxDrawBuffers = 32,
    .maxVertexUniformVectors = 128,
    .maxVaryingVectors = 8,
    .maxFragmentUniformVectors = 16,
    .maxVertexOutputVectors = 16,
    .maxFragmentInputVectors = 15,
    .minProgramTexelOffset = -8,
    .maxProgramTexelOffset = 7,
    .maxClipDistances = 8,
    .maxComputeWorkGroupCountX = 65535,
    .maxComputeWorkGroupCountY = 65535,
    .maxComputeWorkGroupCountZ = 65535,
    .maxComputeWorkGroupSizeX = 1024,
    .maxComputeWorkGroupSizeY = 1024,
    .maxComputeWorkGroupSizeZ = 64,
    .maxComputeUniformComponents = 1024,
    .maxComputeTextureImageUnits = 1 << 16,
    .maxComputeImageUniforms = 1 << 16,
    .maxComputeAtomicCounters = 8,
    .maxComputeAtomicCounterBuffers = 1,
    .maxVaryingComponents = 60,
    .maxVertexOutputComponents = 64,
    .maxGeometryInputComponents = 64,
    .maxGeometryOutputComponents = 128,
    .maxFragmentInputComponents = 128,
    .maxImageUnits = 1 << 16,
    .maxCombinedImageUnitsAndFragmentOutputs = 8,
    .maxCombinedShaderOutputResources = 8,
    .maxImageSamples = 0,
    .maxVertexImageUniforms = 0,
    .maxTessControlImageUniforms = 0,
    .maxTessEvaluationImageUniforms = 0,
    .maxGeometryImageUniforms = 0,
    .maxFragmentImageUniforms = 8,
    .maxCombinedImageUniforms = 8,
    .maxGeometryTextureImageUnits = 16,
    .maxGeometryOutputVertices = 256,
    .maxGeometryTotalOutputComponents = 1024,
    .maxGeometryUniformComponents = 1024,
    .maxGeometryVaryingComponents = 64,
    .maxTessControlInputComponents = 128,
    .maxTessControlOutputComponents = 128,
    .maxTessControlTextureImageUnits = 16,
    .maxTessControlUniformComponents = 1024,
    .maxTessControlTotalOutputComponents = 4096,
    .maxTessEvaluationInputComponents = 128,
    .maxTessEvaluationOutputComponents = 128,
    .maxTessEvaluationTextureImageUnits = 16,
    .maxTessEvaluationUniformComponents = 1024,
    .maxTessPatchComponents = 120,
    .maxPatchVertices = 32,
    .maxTessGenLevel = 64,
    .maxViewports = 16,
    .maxVertexAtomicCounters = 0,
    .maxTessControlAtomicCounters = 0,
    .maxTessEvaluationAtomicCounters = 0,
    .maxGeometryAtomicCounters = 0,
    .maxFragmentAtomicCounters = 8,
    .maxCombinedAtomicCounters = 8,
    .maxAtomicCounterBindings = 1,
    .maxVertexAtomicCounterBuffers = 0,
    .maxTessControlAtomicCounterBuffers = 0,
    .maxTessEvaluationAtomicCounterBuffers = 0,
    .maxGeometryAtomicCounterBuffers = 0,
    .maxFragmentAtomicCounterBuffers = 1,
    .maxCombinedAtomicCounterBuffers = 1,
    .maxAtomicCounterBufferSize = 16384,
    .maxTransformFeedbackBuffers = 4,
    .maxTransformFeedbackInterleavedComponents = 64,
    .maxCullDistances = 8,
    .maxCombinedClipAndCullDistances = 8,
    .maxSamples = 4,
    .maxMeshOutputVerticesNV = 256,
    .maxMeshOutputPrimitivesNV = 512,
    .maxMeshWorkGroupSizeX_NV = 32,
    .maxMeshWorkGroupSizeY_NV = 1,
    .maxMeshWorkGroupSizeZ_NV = 1,
    .maxTaskWorkGroupSizeX_NV = 32,
    .maxTaskWorkGroupSizeY_NV = 1,
    .maxTaskWorkGroupSizeZ_NV = 1,
    .maxMeshViewCountNV = 4,
    .limits{
        .nonInductiveForLoops = 1,
        .whileLoops = 1,
        .doWhileLoops = 1,
        .generalUniformIndexing = 1,
        .generalAttributeMatrixVectorIndexing = 1,
        .generalVaryingIndexing = 1,
        .generalSamplerIndexing = 1,
        .generalVariableIndexing = 1,
        .generalConstantMatrixVectorIndexing = 1,
    },
};

struct VulkanApp {
    bool is_minimized;

    struct Vk {
        struct Queue {
            VkQueue handle;
            uint32_t family_index;
        };
        struct PerFrameData {
            VkFence fence;
            VkSemaphore image_acquired_semaphore;
            VkSemaphore draw_complete_semaphore;
            VkSemaphore image_ownership_semaphore;
        };

        struct Instance {
            VkInstance handle;
            std::vector<const char *> extension_names;
            std::vector<const char *> enabled_layers;
        };
        struct Device {
            VkDevice handle;
            std::vector<const char *> extension_names;
            std::vector<const char *> enabled_layers;
        };
        struct Surface {
            VkSurfaceKHR handle;
            VkSurfaceFormatKHR format;
        };
        struct Swapchain {
            struct ImageResources {
                VkImage image;
                VkCommandBuffer cmd;
                VkCommandBuffer graphics_to_present_cmd;
                VkImageView view;
                VkFramebuffer framebuffer;
            };
            struct Depth {
                VkFormat format;
                VkImage image;
                VkMemoryAllocateInfo mem_alloc;
                VkDeviceMemory mem;
                VkImageView view;
            };
            VkSwapchainKHR handle;
            uint32_t size_x, size_y;
            VkPresentModeKHR present_mode;
            std::vector<ImageResources> image_resources;
            Depth depth;
        };

        struct Pipeline {
            VkPipeline handle;
            VkPipelineLayout layout;
            VkPipelineCache cache;
            VkShaderModule vert_shader_module;
            VkShaderModule frag_shader_module;
            bool valid;
        };

        Instance instance;
        Device device;
        Surface surface;
        Swapchain swapchain;
        Pipeline pipeline;

        VkPhysicalDevice physical_device;
        VkPhysicalDeviceMemoryProperties memory_properties;
        Queue graphics_queue, present_queue;
        bool separate_present_queue;
        std::vector<VkQueueFamilyProperties> queue_props;
        VkCommandPool cmd_pool;
        VkCommandPool present_cmd_pool;
        VkCommandBuffer cmd;
        uint32_t current_buffer;
        VkRenderPass render_pass;
        static constexpr auto FRAME_LAG = 3;
        PerFrameData per_frame_data[FRAME_LAG];
        int32_t frame_index;
        int32_t current_frame_index;
        int32_t frame_count;
        bool prepared;

        VkBuffer scene_vbo;
        VkDeviceMemory scene_vbo_memory;

        VkBuffer ui_vbo;
        VkDeviceMemory ui_vbo_memory;

        VkBuffer ubo;
        VkDeviceMemory ubo_memory;
        VkDescriptorPool desc_pool;
        VkDescriptorSetLayout desc_layout;
        VkDescriptorSet descriptor_set;
    } vk;

    void init(AppState &, WindowHandle w) {
        volkInitialize();
        create_instance();
        volkLoadInstance(vk.instance.handle);
        select_physical_device();
        create_surface(w);
        select_queues();
        create_device();
        volkLoadDevice(vk.device.handle);
        create_swapchain();

        init_scene();

        vk.cmd_pool = VK_NULL_HANDLE;
        vk.swapchain.handle = VK_NULL_HANDLE;
        vk.pipeline.handle = VK_NULL_HANDLE;
        vk.pipeline.cache = VK_NULL_HANDLE;
        prepare();
    }
    void deinit(AppState &, WindowHandle) {
        vk.prepared = false;
        vkDeviceWaitIdle(vk.device.handle);

        deinit_scene();

        for (size_t i = 0; i < vk.swapchain.image_resources.size(); i++)
            vkDestroyFramebuffer(vk.device.handle, vk.swapchain.image_resources[i].framebuffer, nullptr);

        vkDestroyDescriptorPool(vk.device.handle, vk.desc_pool, nullptr);
        vkDestroyDescriptorSetLayout(vk.device.handle, vk.desc_layout, nullptr);

        vkDestroyPipeline(vk.device.handle, vk.pipeline.handle, nullptr);
        vkDestroyPipelineCache(vk.device.handle, vk.pipeline.cache, nullptr);

        vkDestroyRenderPass(vk.device.handle, vk.render_pass, nullptr);
        vkDestroyPipelineLayout(vk.device.handle, vk.pipeline.layout, nullptr);

        vkDestroyImageView(vk.device.handle, vk.swapchain.depth.view, nullptr);
        vkFreeMemory(vk.device.handle, vk.swapchain.depth.mem, nullptr);
        vkDestroyImage(vk.device.handle, vk.swapchain.depth.image, nullptr);

        vkDestroyCommandPool(vk.device.handle, vk.cmd_pool, nullptr);

        for (size_t i = 0; i < vk.swapchain.image_resources.size(); i++)
            vkDestroyImageView(vk.device.handle, vk.swapchain.image_resources[i].view, nullptr);
        vkDestroySwapchainKHR(vk.device.handle, vk.swapchain.handle, nullptr);

        for (uint32_t i = 0; i < Vk::FRAME_LAG; i++) {
            vkDestroyFence(vk.device.handle, vk.per_frame_data[i].fence, nullptr);
            vkDestroySemaphore(vk.device.handle, vk.per_frame_data[i].image_acquired_semaphore, nullptr);
            vkDestroySemaphore(vk.device.handle, vk.per_frame_data[i].draw_complete_semaphore, nullptr);
            if (vk.separate_present_queue)
                vkDestroySemaphore(vk.device.handle, vk.per_frame_data[i].image_ownership_semaphore, nullptr);
        }
        vkDestroySurfaceKHR(vk.instance.handle, vk.surface.handle, nullptr);
        vkDestroyDevice(vk.device.handle, nullptr);
        vkDestroyInstance(vk.instance.handle, nullptr);
    }
    bool update(AppState &app_state, WindowHandle w) {
        vk.swapchain.size_x = app_state.size_x;
        vk.swapchain.size_y = app_state.size_y;
        {
            SceneVertex *mapped_vertices;
            vkMapMemory(vk.device.handle, vk.scene_vbo_memory, 0, sizeof(scene_vertices), 0, reinterpret_cast<void **>(&mapped_vertices));
            for (size_t i = 0; i < scene_vertices.size(); ++i)
                mapped_vertices[i].pos = scene_vertices[i].pos + f32vec2{app_state.mouse_x, app_state.mouse_y};
            vkUnmapMemory(vk.device.handle, vk.scene_vbo_memory);
        }
        if (!vk.prepared) {
            using namespace std::literals;
            std::this_thread::sleep_for(1ms);
            return true;
        }
        draw(w);
        return true;
    }

    static bool memory_type_from_properties(auto &memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) {
        for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
            if ((typeBits & 1) == 1) {
                if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                    *typeIndex = i;
                    return true;
                }
            }
            typeBits >>= 1;
        }
        return false;
    };

    void init_scene() {
        {
            void *data_ptr = scene_vertices.data();
            size_t data_size = sizeof(scene_vertices);

            VkBufferCreateInfo buffer_ci{
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .size = data_size,
                .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            };
            vkCreateBuffer(vk.device.handle, &buffer_ci, nullptr, &vk.scene_vbo);
            VkMemoryRequirements memory_requirements;
            vkGetBufferMemoryRequirements(vk.device.handle, vk.scene_vbo, &memory_requirements);
            uint32_t type_index;
            memory_type_from_properties(vk.memory_properties, memory_requirements.memoryTypeBits,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &type_index);
            VkMemoryAllocateInfo alloc_info{
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .allocationSize = memory_requirements.size,
                .memoryTypeIndex = type_index,
            };
            vkAllocateMemory(vk.device.handle, &alloc_info, nullptr, &vk.scene_vbo_memory);
            vkBindBufferMemory(vk.device.handle, vk.scene_vbo, vk.scene_vbo_memory, 0);
            void *mapped_region;
            vkMapMemory(vk.device.handle, vk.scene_vbo_memory, 0, data_size, 0, &mapped_region);
            memcpy(mapped_region, data_ptr, data_size);
            vkUnmapMemory(vk.device.handle, vk.scene_vbo_memory);
        }
        {
            void *data_ptr = ui_vertices.data();
            size_t data_size = sizeof(ui_vertices);

            VkBufferCreateInfo buffer_ci{
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .size = data_size,
                .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            };
            vkCreateBuffer(vk.device.handle, &buffer_ci, nullptr, &vk.ui_vbo);
            VkMemoryRequirements memory_requirements;
            vkGetBufferMemoryRequirements(vk.device.handle, vk.ui_vbo, &memory_requirements);
            uint32_t type_index;
            memory_type_from_properties(vk.memory_properties, memory_requirements.memoryTypeBits,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &type_index);
            VkMemoryAllocateInfo alloc_info{
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .allocationSize = memory_requirements.size,
                .memoryTypeIndex = type_index,
            };
            vkAllocateMemory(vk.device.handle, &alloc_info, nullptr, &vk.ui_vbo_memory);
            vkBindBufferMemory(vk.device.handle, vk.ui_vbo, vk.ui_vbo_memory, 0);
            void *mapped_region;
            vkMapMemory(vk.device.handle, vk.ui_vbo_memory, 0, data_size, 0, &mapped_region);
            memcpy(mapped_region, data_ptr, data_size);
            vkUnmapMemory(vk.device.handle, vk.ui_vbo_memory);
        }

        uniforms.view = scale(f32mat4::identity(), {1, 1, 1});
        uniforms.ui_view = scale(translate(f32mat4::identity(), {0, 0, 0}), {1, 1, 1});

        {
            void *data_ptr = &uniforms;
            size_t data_size = sizeof(uniforms);

            VkBufferCreateInfo buffer_ci{
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .size = data_size,
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            };
            vkCreateBuffer(vk.device.handle, &buffer_ci, nullptr, &vk.ubo);
            VkMemoryRequirements memory_requirements;
            vkGetBufferMemoryRequirements(vk.device.handle, vk.ubo, &memory_requirements);
            uint32_t type_index;
            memory_type_from_properties(vk.memory_properties, memory_requirements.memoryTypeBits,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &type_index);
            VkMemoryAllocateInfo alloc_info{
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .allocationSize = memory_requirements.size,
                .memoryTypeIndex = type_index,
            };
            vkAllocateMemory(vk.device.handle, &alloc_info, nullptr, &vk.ubo_memory);
            vkBindBufferMemory(vk.device.handle, vk.ubo, vk.ubo_memory, 0);
            void *mapped_region;
            vkMapMemory(vk.device.handle, vk.ubo_memory, 0, data_size, 0, &mapped_region);
            memcpy(mapped_region, data_ptr, data_size);
            vkUnmapMemory(vk.device.handle, vk.ubo_memory);
        }
    }
    void deinit_scene() {
        vkDestroyBuffer(vk.device.handle, vk.scene_vbo, nullptr);
        vkFreeMemory(vk.device.handle, vk.scene_vbo_memory, nullptr);
        vkDestroyBuffer(vk.device.handle, vk.ui_vbo, nullptr);
        vkFreeMemory(vk.device.handle, vk.ui_vbo_memory, nullptr);

        vkDestroyBuffer(vk.device.handle, vk.ubo, nullptr);
        vkFreeMemory(vk.device.handle, vk.ubo_memory, nullptr);
    }
    void create_instance() {
        const VkApplicationInfo app_info = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "Vulkan App",
            .applicationVersion = 0,
            .pEngineName = "coel",
            .engineVersion = 0,
            .apiVersion = VK_API_VERSION_1_0,
        };

        vk.instance.enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
        vk.instance.extension_names.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if COEL_USE_WIN32
        vk.instance.extension_names.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif COEL_USE_X11
        vk.instance.extension_names.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif COEL_USE_NULL
#endif

        {
            auto check_layers = [](auto &&required_names, auto &&layer_props) -> bool {
                for (auto &required_layer_name : required_names) {
                    bool found = false;
                    for (auto &existing_layer_prop : layer_props) {
                        if (!strcmp(required_layer_name, existing_layer_prop.layerName)) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        std::cerr << "Cannot find layer: " << required_layer_name << std::endl;
                        return false;
                    }
                }
                return true;
            };

            std::vector<VkLayerProperties> instance_layers;
            uint32_t instance_layer_count;
            vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr);
            instance_layers.resize(instance_layer_count);
            vkEnumerateInstanceLayerProperties(&instance_layer_count, instance_layers.data());
            check_layers(vk.instance.enabled_layers, instance_layers);
        }

        VkInstanceCreateInfo instance_ci = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .pApplicationInfo = &app_info,
            .enabledLayerCount = static_cast<uint32_t>(vk.instance.enabled_layers.size()),
            .ppEnabledLayerNames = vk.instance.enabled_layers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(vk.instance.extension_names.size()),
            .ppEnabledExtensionNames = vk.instance.extension_names.data(),
        };
        vkCreateInstance(&instance_ci, nullptr, &vk.instance.handle);
    }
    void select_physical_device() {
        uint32_t physical_device_n;
        vkEnumeratePhysicalDevices(vk.instance.handle, &physical_device_n, nullptr);
        std::vector<VkPhysicalDevice> physical_devices;
        physical_devices.resize(physical_device_n);
        vkEnumeratePhysicalDevices(vk.instance.handle, &physical_device_n, physical_devices.data());
        vk.physical_device = physical_devices[0];
    }
    void create_surface(WindowHandle w) {
#if COEL_USE_WIN32
        VkWin32SurfaceCreateInfoKHR surface_ci{
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .hinstance = GetModuleHandleA(nullptr),
            .hwnd = w->hwnd,
        };
        vkCreateWin32SurfaceKHR(vk.instance.handle, &surface_ci, nullptr, &vk.surface.handle);
#elif COEL_USE_X11
        VkXlibSurfaceCreateInfoKHR surface_ci{
            .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .dpy = nullptr,
            .window = w->xid,
        };
        vkCreateXlibSurfaceKHR(vk.instance.handle, &surface_ci, nullptr, &vk.surface.handle);
#endif
    }
    void select_queues() {
        uint32_t queue_family_count;
        vkGetPhysicalDeviceQueueFamilyProperties(vk.physical_device, &queue_family_count, nullptr);
        vk.queue_props.resize(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(vk.physical_device, &queue_family_count, vk.queue_props.data());
        std::vector<VkBool32> supports_present;
        supports_present.resize(queue_family_count);
        for (uint32_t i = 0; i < queue_family_count; i++)
            vkGetPhysicalDeviceSurfaceSupportKHR(vk.physical_device, i, vk.surface.handle, &supports_present[i]);
        uint32_t graphics_queue_family_index = UINT32_MAX;
        uint32_t present_queue_family_index = UINT32_MAX;
        for (uint32_t i = 0; i < queue_family_count; i++) {
            if ((vk.queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
                if (graphics_queue_family_index == UINT32_MAX) {
                    graphics_queue_family_index = i;
                }
                if (supports_present[i] == VK_TRUE) {
                    present_queue_family_index = i;
                    present_queue_family_index = i;
                    break;
                }
            }
        }
        if (present_queue_family_index == UINT32_MAX) {
            for (uint32_t i = 0; i < queue_family_count; ++i) {
                if (supports_present[i] == VK_TRUE) {
                    present_queue_family_index = i;
                    break;
                }
            }
        }
        if (graphics_queue_family_index == UINT32_MAX ||
            present_queue_family_index == UINT32_MAX) {
            std::cerr << "Could not find both graphics and present queues\n"
                      << "Swapchain Initialization Failure" << std::endl;
        }
        vk.graphics_queue.family_index = graphics_queue_family_index;
        vk.present_queue.family_index = present_queue_family_index;
        vk.separate_present_queue = (vk.graphics_queue.family_index != vk.present_queue.family_index);
    }
    void create_device() {
        float queue_priorities[1] = {0.0};
        VkDeviceQueueCreateInfo queues[2];
        queues[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queues[0].pNext = nullptr;
        queues[0].queueFamilyIndex = vk.graphics_queue.family_index;
        queues[0].queueCount = 1;
        queues[0].pQueuePriorities = queue_priorities;
        queues[0].flags = 0;
        vk.device.extension_names.clear();
        vk.device.extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        VkDeviceCreateInfo device_ci = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = queues,
            .enabledLayerCount = static_cast<uint32_t>(vk.device.enabled_layers.size()),
            .ppEnabledLayerNames = vk.device.enabled_layers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(vk.device.extension_names.size()),
            .ppEnabledExtensionNames = vk.device.extension_names.data(),
            .pEnabledFeatures = nullptr,
        };
        if (vk.separate_present_queue) {
            queues[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queues[1].pNext = nullptr;
            queues[1].queueFamilyIndex = vk.present_queue.family_index;
            queues[1].queueCount = 1;
            queues[1].pQueuePriorities = queue_priorities;
            queues[1].flags = 0;
            device_ci.queueCreateInfoCount = 2;
        }
        vkCreateDevice(vk.physical_device, &device_ci, nullptr, &vk.device.handle);
        vkGetDeviceQueue(vk.device.handle, vk.graphics_queue.family_index, 0, &vk.graphics_queue.handle);
        if (!vk.separate_present_queue) {
            vk.present_queue = vk.graphics_queue;
        } else {
            vkGetDeviceQueue(vk.device.handle, vk.present_queue.family_index, 0, &vk.present_queue.handle);
        }
    }
    void create_swapchain() {
        uint32_t format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(vk.physical_device, vk.surface.handle, &format_count, nullptr);
        std::vector<VkSurfaceFormatKHR> surface_formats;
        surface_formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(vk.physical_device, vk.surface.handle, &format_count, surface_formats.data());
        auto pick_surface_format = [](const std::vector<VkSurfaceFormatKHR> &surface_formats) -> VkSurfaceFormatKHR {
            for (const auto &surface_format : surface_formats) {
                const VkFormat format = surface_format.format;
                if (format == VK_FORMAT_R8G8B8A8_UNORM ||
                    format == VK_FORMAT_B8G8R8A8_UNORM ||
                    format == VK_FORMAT_A2B10G10R10_UNORM_PACK32 ||
                    format == VK_FORMAT_A2R10G10B10_UNORM_PACK32 ||
                    format == VK_FORMAT_R16G16B16A16_SFLOAT) {
                    return surface_format;
                }
            }
            std::cout << "Can't find our preferred formats... Falling back to first exposed format. Rendering may be incorrect." << std::endl;
            return surface_formats[0];
        };
        vk.surface.format = pick_surface_format(surface_formats);
        vk.current_frame_index = 0;
        VkSemaphoreCreateInfo semaphore_ci = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
        };
        VkFenceCreateInfo fence_ci = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };
        for (uint32_t i = 0; i < Vk::FRAME_LAG; i++) {
            vkCreateFence(vk.device.handle, &fence_ci, nullptr, &vk.per_frame_data[i].fence);
            vkCreateSemaphore(vk.device.handle, &semaphore_ci, nullptr, &vk.per_frame_data[i].image_acquired_semaphore);
            vkCreateSemaphore(vk.device.handle, &semaphore_ci, nullptr, &vk.per_frame_data[i].draw_complete_semaphore);
            if (vk.separate_present_queue)
                vkCreateSemaphore(vk.device.handle, &semaphore_ci, nullptr, &vk.per_frame_data[i].image_ownership_semaphore);
        }
        vk.frame_index = 0;
        vkGetPhysicalDeviceMemoryProperties(vk.physical_device, &vk.memory_properties);
    }
    void prepare() {
        prepare_buffers();
        if (is_minimized) {
            vk.prepared = false;
            return;
        }
        prepare_init_cmd();
        begin_init_cmd();
        prepare_depth();
        prepare_textures();
        prepare_data_buffers();
        prepare_descriptor_layout();
        prepare_render_pass();
        if (vk.pipeline.handle == VK_NULL_HANDLE)
            prepare_pipeline();
        prepare_command_buffers();
        prepare_descriptor_pool();
        prepare_descriptor_set();
        prepare_framebuffers();
        for (uint32_t i = 0; i < vk.swapchain.image_resources.size(); i++) {
            vk.current_buffer = i;
            prepare_draw_cmd(vk.swapchain.image_resources[i].cmd);
        }
        flush_init_cmd();
        vk.prepared = true;
        vk.current_buffer = 0;
    }
    void prepare_buffers() {
        VkSwapchainKHR old_swapchain = vk.swapchain.handle;
        VkSurfaceCapabilitiesKHR surface_capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk.physical_device, vk.surface.handle, &surface_capabilities);
        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(vk.physical_device, vk.surface.handle, &present_mode_count, nullptr);
        std::vector<VkPresentModeKHR> present_modes;
        present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(vk.physical_device, vk.surface.handle, &present_mode_count, present_modes.data());
        VkExtent2D swapchain_extent;
        if (surface_capabilities.currentExtent.width == 0xFFFFFFFF) {
            swapchain_extent.width = vk.swapchain.size_x;
            swapchain_extent.height = vk.swapchain.size_y;
            if (swapchain_extent.width < surface_capabilities.minImageExtent.width)
                swapchain_extent.width = surface_capabilities.minImageExtent.width;
            else if (swapchain_extent.width > surface_capabilities.maxImageExtent.width)
                swapchain_extent.width = surface_capabilities.maxImageExtent.width;
            if (swapchain_extent.height < surface_capabilities.minImageExtent.height)
                swapchain_extent.height = surface_capabilities.minImageExtent.height;
            else if (swapchain_extent.height > surface_capabilities.maxImageExtent.height)
                swapchain_extent.height = surface_capabilities.maxImageExtent.height;
        } else {
            swapchain_extent = surface_capabilities.currentExtent;
            vk.swapchain.size_x = surface_capabilities.currentExtent.width;
            vk.swapchain.size_y = surface_capabilities.currentExtent.height;
        }
        if (surface_capabilities.maxImageExtent.width == 0 || surface_capabilities.maxImageExtent.height == 0) {
            is_minimized = true;
            return;
        } else {
            is_minimized = false;
        }
        VkPresentModeKHR swapchain_present_mode = VK_PRESENT_MODE_FIFO_KHR;
        vk.swapchain.present_mode = VK_PRESENT_MODE_FIFO_KHR;
        if (vk.swapchain.present_mode != swapchain_present_mode) {
            for (size_t i = 0; i < present_mode_count; ++i) {
                if (present_modes[i] == vk.swapchain.present_mode) {
                    swapchain_present_mode = vk.swapchain.present_mode;
                    break;
                }
            }
        }
        if (swapchain_present_mode != vk.swapchain.present_mode) {
            std::cerr << "Present mode specified is not supported" << std::endl;
        }
        uint32_t desired_image_n = 3;
        if (desired_image_n < surface_capabilities.minImageCount)
            desired_image_n = surface_capabilities.minImageCount;
        if ((surface_capabilities.maxImageCount > 0) && (desired_image_n > surface_capabilities.maxImageCount))
            desired_image_n = surface_capabilities.maxImageCount;
        VkSurfaceTransformFlagBitsKHR preTransform;
        if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
            preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        else
            preTransform = surface_capabilities.currentTransform;
        VkCompositeAlphaFlagBitsKHR composite_alpha =
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        VkCompositeAlphaFlagBitsKHR composite_alpha_flags[4] = {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        };
        for (auto alpha_flag : composite_alpha_flags) {
            if (surface_capabilities.supportedCompositeAlpha & alpha_flag) {
                composite_alpha = alpha_flag;
                break;
            }
        }
        VkSwapchainCreateInfoKHR swapchain_ci = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .surface = vk.surface.handle,
            .minImageCount = desired_image_n,
            .imageFormat = vk.surface.format.format,
            .imageColorSpace = vk.surface.format.colorSpace,
            .imageExtent = {
                .width = swapchain_extent.width,
                .height = swapchain_extent.height,
            },
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .preTransform = preTransform,
            .compositeAlpha = composite_alpha,
            .presentMode = swapchain_present_mode,
            .clipped = true,
            .oldSwapchain = old_swapchain,
        };
        uint32_t i;
        vkCreateSwapchainKHR(vk.device.handle, &swapchain_ci, nullptr, &vk.swapchain.handle);
        if (old_swapchain != VK_NULL_HANDLE)
            vkDestroySwapchainKHR(vk.device.handle, old_swapchain, nullptr);
        uint32_t image_count;
        vkGetSwapchainImagesKHR(vk.device.handle, vk.swapchain.handle, &image_count, nullptr);
        std::vector<VkImage> swapchain_images;
        swapchain_images.resize(image_count);
        vkGetSwapchainImagesKHR(vk.device.handle, vk.swapchain.handle, &image_count, swapchain_images.data());
        vk.swapchain.image_resources.resize(image_count);
        for (i = 0; i < vk.swapchain.image_resources.size(); i++) {
            VkImageViewCreateInfo color_image_view = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = vk.surface.format.format,
                .components = {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                },
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            };
            vk.swapchain.image_resources[i].image = swapchain_images[i];
            color_image_view.image = vk.swapchain.image_resources[i].image;
            vkCreateImageView(vk.device.handle, &color_image_view, nullptr, &vk.swapchain.image_resources[i].view);
        }
    }
    void prepare_init_cmd() {
        if (vk.cmd_pool == VK_NULL_HANDLE) {
            const VkCommandPoolCreateInfo cmd_pool_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = vk.graphics_queue.family_index,
            };
            vkCreateCommandPool(vk.device.handle, &cmd_pool_info, nullptr, &vk.cmd_pool);
        }
        const VkCommandBufferAllocateInfo cmd = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = vk.cmd_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };
        vkAllocateCommandBuffers(vk.device.handle, &cmd, &vk.cmd);
        // VkCommandBufferBeginInfo cmd_buf_info = {
        //     .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        //     .pNext = nullptr,
        //     .flags = 0,
        //     .pInheritanceInfo = nullptr,
        // };
    }
    void prepare_depth() {
        const VkFormat depth_format = VK_FORMAT_D16_UNORM;
        const VkImageCreateInfo image = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = depth_format,
            .extent = {static_cast<uint32_t>(vk.swapchain.size_x), static_cast<uint32_t>(vk.swapchain.size_y), 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        };
        VkImageViewCreateInfo view = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = VK_NULL_HANDLE,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = depth_format,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
        VkMemoryRequirements mem_reqs;
        // VkResult err;
        // bool pass;
        vk.swapchain.depth.format = depth_format;
        vkCreateImage(vk.device.handle, &image, nullptr, &vk.swapchain.depth.image);
        vkGetImageMemoryRequirements(vk.device.handle, vk.swapchain.depth.image, &mem_reqs);
        vk.swapchain.depth.mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vk.swapchain.depth.mem_alloc.pNext = nullptr;
        vk.swapchain.depth.mem_alloc.allocationSize = mem_reqs.size;
        vk.swapchain.depth.mem_alloc.memoryTypeIndex = 0;
        memory_type_from_properties(vk.memory_properties, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk.swapchain.depth.mem_alloc.memoryTypeIndex);
        vkAllocateMemory(vk.device.handle, &vk.swapchain.depth.mem_alloc, nullptr, &vk.swapchain.depth.mem);
        vkBindImageMemory(vk.device.handle, vk.swapchain.depth.image, vk.swapchain.depth.mem, 0);
        view.image = vk.swapchain.depth.image;
        vkCreateImageView(vk.device.handle, &view, nullptr, &vk.swapchain.depth.view);
    }
    void prepare_textures() {
    }
    void prepare_data_buffers() {
    }
    void prepare_descriptor_layout() {
        const VkDescriptorSetLayoutBinding layout_bindings[] = {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr,
            },
        };
        const VkDescriptorSetLayoutCreateInfo desc_layout_ci = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .bindingCount = 1,
            .pBindings = layout_bindings,
        };
        vkCreateDescriptorSetLayout(vk.device.handle, &desc_layout_ci, nullptr, &vk.desc_layout);
        const VkPipelineLayoutCreateInfo pipeline_layout_ci = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .setLayoutCount = 1,
            .pSetLayouts = &vk.desc_layout,
        };
        vkCreatePipelineLayout(vk.device.handle, &pipeline_layout_ci, nullptr, &vk.pipeline.layout);
    }
    void prepare_render_pass() {
        const VkAttachmentDescription attachments[2] = {
            {
                .flags = 0,
                .format = vk.surface.format.format,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            },
            {
                .flags = 0,
                .format = vk.swapchain.depth.format,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            },
        };
        const VkAttachmentReference color_reference = {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };
        const VkAttachmentReference depth_reference = {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };
        const VkSubpassDescription subpass = {
            .flags = 0,
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .inputAttachmentCount = 0,
            .pInputAttachments = nullptr,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_reference,
            .pResolveAttachments = nullptr,
            .pDepthStencilAttachment = &depth_reference,
            .preserveAttachmentCount = 0,
            .pPreserveAttachments = nullptr,
        };
        VkSubpassDependency attachmentDependencies[2] = {
            {
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = 0,
                .srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .dependencyFlags = 0,
            },
            {
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = 0,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
                .dependencyFlags = 0,
            },
        };
        const VkRenderPassCreateInfo renderpass_ci = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .attachmentCount = 2,
            .pAttachments = attachments,
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = 2,
            .pDependencies = attachmentDependencies,
        };
        vkCreateRenderPass(vk.device.handle, &renderpass_ci, nullptr, &vk.render_pass);
    }
    void prepare_pipeline() {
        constexpr auto NUM_DYNAMIC_STATES = 2;
        VkGraphicsPipelineCreateInfo pipeline;
        VkPipelineCacheCreateInfo pipelineCache;
        VkPipelineVertexInputStateCreateInfo vi;
        VkPipelineInputAssemblyStateCreateInfo ia;
        VkPipelineRasterizationStateCreateInfo rs;
        VkPipelineColorBlendStateCreateInfo cb;
        VkPipelineDepthStencilStateCreateInfo ds;
        VkPipelineViewportStateCreateInfo vp;
        VkPipelineMultisampleStateCreateInfo ms;
        VkDynamicState dynamicStateEnables[NUM_DYNAMIC_STATES];
        VkPipelineDynamicStateCreateInfo dynamicState;
        memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
        memset(&dynamicState, 0, sizeof dynamicState);
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.pDynamicStates = dynamicStateEnables;
        memset(&pipeline, 0, sizeof(pipeline));
        pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline.layout = vk.pipeline.layout;
        memset(&vi, 0, sizeof(vi));
        vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vi.pNext = nullptr;
        VkVertexInputBindingDescription bindings[] = {
            {
                .binding = 0,
                .stride = sizeof(SceneVertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
            },
        };
        VkVertexInputAttributeDescription attribs[] = {
            {
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = 0,
            },
            {
                .location = 1,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = 2 * sizeof(float),
            },
        };
        vi.vertexBindingDescriptionCount = 1;
        vi.pVertexBindingDescriptions = bindings;
        vi.vertexAttributeDescriptionCount = 2;
        vi.pVertexAttributeDescriptions = attribs;
        memset(&ia, 0, sizeof(ia));
        ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        memset(&rs, 0, sizeof(rs));
        rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rs.polygonMode = VK_POLYGON_MODE_FILL;
        rs.cullMode = VK_CULL_MODE_BACK_BIT;
        rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rs.depthClampEnable = VK_FALSE;
        rs.rasterizerDiscardEnable = VK_FALSE;
        rs.depthBiasEnable = VK_FALSE;
        rs.lineWidth = 1.0f;
        memset(&cb, 0, sizeof(cb));
        cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        VkPipelineColorBlendAttachmentState att_state[1];
        memset(att_state, 0, sizeof(att_state));
        att_state[0].colorWriteMask = 0xf;
        att_state[0].blendEnable = VK_FALSE;
        cb.attachmentCount = 1;
        cb.pAttachments = att_state;
        memset(&vp, 0, sizeof(vp));
        vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        vp.viewportCount = 1;
        dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
        vp.scissorCount = 1;
        dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
        memset(&ds, 0, sizeof(ds));
        ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        ds.depthTestEnable = VK_TRUE;
        ds.depthWriteEnable = VK_TRUE;
        ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        ds.depthBoundsTestEnable = VK_FALSE;
        ds.back.failOp = VK_STENCIL_OP_KEEP;
        ds.back.passOp = VK_STENCIL_OP_KEEP;
        ds.back.compareOp = VK_COMPARE_OP_ALWAYS;
        ds.stencilTestEnable = VK_FALSE;
        ds.front = ds.back;
        memset(&ms, 0, sizeof(ms));
        ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        ms.pSampleMask = nullptr;
        ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        auto prepare_shader_module = [](VkDevice device_handle, const char *const code_str, EShLanguage stage) -> VkShaderModule {
            const char *shaderStrings[] = {code_str};
            glslang::TShader shader(stage);
            shader.setStrings(shaderStrings, 1);
            auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);
            TBuiltInResource resource = DEFAULT_SHADER_RESOURCE_SIZES;
            if (!shader.parse(&resource, 100, false, messages)) {
                std::cerr << shader.getInfoLog() << '\n'
                          << shader.getInfoDebugLog() << std::endl;
            }
            glslang::TProgram program;
            program.addShader(&shader);
            if (!program.link(messages)) {
                std::cerr << shader.getInfoLog() << '\n'
                          << shader.getInfoDebugLog() << std::endl;
            }
            std::vector<uint32_t> spv_binary;
            glslang::GlslangToSpv(*program.getIntermediate(stage), spv_binary);

            VkShaderModule module;
            VkShaderModuleCreateInfo moduleCreateInfo;
            moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            moduleCreateInfo.pNext = nullptr;
            moduleCreateInfo.flags = 0;
            moduleCreateInfo.codeSize = spv_binary.size() * sizeof(uint32_t);
            moduleCreateInfo.pCode = spv_binary.data();
            vkCreateShaderModule(device_handle, &moduleCreateInfo, nullptr, &module);
            return module;
        };
        glslang::InitializeProcess();
        vk.pipeline.vert_shader_module = prepare_shader_module(vk.device.handle, scene_vert_src, EShLangVertex);
        vk.pipeline.frag_shader_module = prepare_shader_module(vk.device.handle, scene_frag_src, EShLangFragment);
        glslang::FinalizeProcess();
        VkPipelineShaderStageCreateInfo shaderStages[2];
        memset(&shaderStages, 0, 2 * sizeof(VkPipelineShaderStageCreateInfo));
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vk.pipeline.vert_shader_module;
        shaderStages[0].pName = "main";
        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = vk.pipeline.frag_shader_module;
        shaderStages[1].pName = "main";
        memset(&pipelineCache, 0, sizeof(pipelineCache));
        pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        vkCreatePipelineCache(vk.device.handle, &pipelineCache, nullptr, &vk.pipeline.cache);
        pipeline.pVertexInputState = &vi;
        pipeline.pInputAssemblyState = &ia;
        pipeline.pRasterizationState = &rs;
        pipeline.pColorBlendState = &cb;
        pipeline.pMultisampleState = &ms;
        pipeline.pViewportState = &vp;
        pipeline.pDepthStencilState = &ds;
        pipeline.stageCount = 2;
        pipeline.pStages = shaderStages;
        pipeline.renderPass = vk.render_pass;
        pipeline.pDynamicState = &dynamicState;
        pipeline.renderPass = vk.render_pass;
        vkCreateGraphicsPipelines(vk.device.handle, vk.pipeline.cache, 1, &pipeline, nullptr, &vk.pipeline.handle);
        vkDestroyShaderModule(vk.device.handle, vk.pipeline.frag_shader_module, nullptr);
        vkDestroyShaderModule(vk.device.handle, vk.pipeline.vert_shader_module, nullptr);
    }
    void prepare_command_buffers() {
        const VkCommandBufferAllocateInfo cmd = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = vk.cmd_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };
        for (uint32_t i = 0; i < vk.swapchain.image_resources.size(); i++)
            vkAllocateCommandBuffers(vk.device.handle, &cmd, &vk.swapchain.image_resources[i].cmd);
        if (vk.separate_present_queue) {
            const VkCommandPoolCreateInfo present_cmd_pool_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = vk.present_queue.family_index,
            };
            vkCreateCommandPool(vk.device.handle, &present_cmd_pool_info, nullptr, &vk.present_cmd_pool);
            const VkCommandBufferAllocateInfo present_cmd_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext = nullptr,
                .commandPool = vk.present_cmd_pool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1,
            };

            auto build_image_ownership_cmd = [](auto &graphics_to_present_cmd, auto &image, auto &graphics_queue, auto &present_queue, int) -> void {
                const VkCommandBufferBeginInfo cmd_buf_info = {
                    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                    .pNext = nullptr,
                    .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
                    .pInheritanceInfo = nullptr,
                };
                vkBeginCommandBuffer(graphics_to_present_cmd, &cmd_buf_info);
                VkImageMemoryBarrier image_ownership_barrier = {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .pNext = nullptr,
                    .srcAccessMask = 0,
                    .dstAccessMask = 0,
                    .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    .srcQueueFamilyIndex = graphics_queue.family_index,
                    .dstQueueFamilyIndex = present_queue.family_index,
                    .image = image,
                    .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
                };
                vkCmdPipelineBarrier(graphics_to_present_cmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_ownership_barrier);
                vkEndCommandBuffer(graphics_to_present_cmd);
            };

            for (size_t i = 0; i < vk.swapchain.image_resources.size(); i++) {
                vkAllocateCommandBuffers(vk.device.handle, &present_cmd_info, &vk.swapchain.image_resources[i].graphics_to_present_cmd);
                build_image_ownership_cmd(vk.swapchain.image_resources[i].graphics_to_present_cmd, vk.swapchain.image_resources[i].image, vk.graphics_queue, vk.present_queue, static_cast<int32_t>(i));
            }
        }
    }
    void prepare_descriptor_pool() {
        const VkDescriptorPoolSize type_counts[] = {
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
            },
        };
        const VkDescriptorPoolCreateInfo descriptor_pool_ci = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .maxSets = 1,
            .poolSizeCount = 1,
            .pPoolSizes = type_counts,
        };
        vkCreateDescriptorPool(vk.device.handle, &descriptor_pool_ci, nullptr, &vk.desc_pool);
    }
    void prepare_descriptor_set() {
        VkWriteDescriptorSet writes[1];
        VkDescriptorSetAllocateInfo alloc_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = vk.desc_pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &vk.desc_layout,
        };
        VkDescriptorBufferInfo buffer_info;
        buffer_info.offset = 0;
        buffer_info.range = sizeof(Uniforms);
        memset(&writes, 0, sizeof(writes));
        writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[0].descriptorCount = 1;
        writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writes[0].pBufferInfo = &buffer_info;
        vkAllocateDescriptorSets(vk.device.handle, &alloc_info, &vk.descriptor_set);
        buffer_info.buffer = vk.ubo;
        writes[0].dstSet = vk.descriptor_set;
        vkUpdateDescriptorSets(vk.device.handle, 1, writes, 0, nullptr);
    }
    void prepare_framebuffers() {
        VkImageView attachments[2];
        attachments[1] = vk.swapchain.depth.view;
        const VkFramebufferCreateInfo fb_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .renderPass = vk.render_pass,
            .attachmentCount = 2,
            .pAttachments = attachments,
            .width = static_cast<uint32_t>(vk.swapchain.size_x),
            .height = static_cast<uint32_t>(vk.swapchain.size_y),
            .layers = 1,
        };
        uint32_t i;
        for (i = 0; i < vk.swapchain.image_resources.size(); i++) {
            attachments[0] = vk.swapchain.image_resources[i].view;
            vkCreateFramebuffer(vk.device.handle, &fb_info, nullptr, &vk.swapchain.image_resources[i].framebuffer);
        }
    }
    void prepare_draw_cmd(VkCommandBuffer &cmd_buf) {
        VkDebugUtilsLabelEXT label;
        memset(&label, 0, sizeof(label));
        const VkCommandBufferBeginInfo cmd_buf_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
            .pInheritanceInfo = nullptr,
        };
        const VkClearValue clear_values[2] = {
            {.color{.float32 = {0.16f, 0.08f, 0.08f, 1.0f}}},
            {.depthStencil = {1.0f, 0}},
        };
        const VkRenderPassBeginInfo rp_begin = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = vk.render_pass,
            .framebuffer = vk.swapchain.image_resources[vk.current_buffer].framebuffer,
            .renderArea{
                .offset{.x = 0, .y = 0},
                .extent{.width = static_cast<uint32_t>(vk.swapchain.size_x), .height = static_cast<uint32_t>(vk.swapchain.size_y)},
            },
            .clearValueCount = 2,
            .pClearValues = clear_values,
        };
        VkResult err;
        err = vkBeginCommandBuffer(cmd_buf, &cmd_buf_info);
        assert(!err);
        vkCmdBeginRenderPass(cmd_buf, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, vk.pipeline.handle);
        vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, vk.pipeline.layout, 0, 1, &vk.descriptor_set, 0, nullptr);
        VkViewport viewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(vk.swapchain.size_x),
            .height = static_cast<float>(vk.swapchain.size_y),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };
        vkCmdSetViewport(cmd_buf, 0, 1, &viewport);
        VkRect2D scissor{
            .offset = {0, 0},
            .extent = {vk.swapchain.size_x, vk.swapchain.size_y},
        };
        vkCmdSetScissor(cmd_buf, 0, 1, &scissor);
        VkDeviceSize offsets[]{0};
        vkCmdBindVertexBuffers(cmd_buf, 0, 1, &vk.scene_vbo, offsets);
        vkCmdDraw(cmd_buf, scene_vertices.size(), 1, 0, 0);
        vkCmdBindVertexBuffers(cmd_buf, 0, 1, &vk.ui_vbo, offsets);
        vkCmdDraw(cmd_buf, ui_vertices.size(), 1, 0, 0);
        vkCmdEndRenderPass(cmd_buf);
        if (vk.separate_present_queue) {
            VkImageMemoryBarrier image_ownership_barrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .pNext = nullptr,
                .srcAccessMask = 0,
                .dstAccessMask = 0,
                .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .srcQueueFamilyIndex = vk.graphics_queue.family_index,
                .dstQueueFamilyIndex = vk.present_queue.family_index,
                .image = vk.swapchain.image_resources[vk.current_buffer].image,
                .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
            };
            vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_ownership_barrier);
        }
        err = vkEndCommandBuffer(cmd_buf);
        assert(!err);
    }
    void begin_init_cmd() {
        VkCommandBufferBeginInfo cmd_buf_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr,
        };
        vkBeginCommandBuffer(vk.cmd, &cmd_buf_info);
    }
    void flush_init_cmd() {
        if (vk.cmd == VK_NULL_HANDLE)
            return;
        vkEndCommandBuffer(vk.cmd);
        VkFence fence;
        VkFenceCreateInfo fence_ci = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
        };
        vkCreateFence(vk.device.handle, &fence_ci, nullptr, &fence);
        const VkCommandBuffer cmd_bufs[] = {vk.cmd};
        VkSubmitInfo submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = cmd_bufs,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr,
        };
        vkQueueSubmit(vk.graphics_queue.handle, 1, &submit_info, fence);
        vkWaitForFences(vk.device.handle, 1, &fence, VK_TRUE, UINT64_MAX);
        vkFreeCommandBuffers(vk.device.handle, vk.cmd_pool, 1, cmd_bufs);
        vkDestroyFence(vk.device.handle, fence, nullptr);
        vk.cmd = VK_NULL_HANDLE;
    }
    void resize() {
        uint32_t i;
        if (!vk.prepared) {
            if (is_minimized)
                prepare();
            return;
        }
        vk.prepared = false;
        vkDeviceWaitIdle(vk.device.handle);

        for (i = 0; i < vk.swapchain.image_resources.size(); i++)
            vkDestroyFramebuffer(vk.device.handle, vk.swapchain.image_resources[i].framebuffer, nullptr);
        vkDestroyDescriptorPool(vk.device.handle, vk.desc_pool, nullptr);
        vkDestroyDescriptorSetLayout(vk.device.handle, vk.desc_layout, nullptr);
        vkDestroyRenderPass(vk.device.handle, vk.render_pass, nullptr);
        vkDestroyImageView(vk.device.handle, vk.swapchain.depth.view, nullptr);
        vkDestroyImage(vk.device.handle, vk.swapchain.depth.image, nullptr);
        vkFreeMemory(vk.device.handle, vk.swapchain.depth.mem, nullptr);
        for (i = 0; i < vk.swapchain.image_resources.size(); i++) {
            vkDestroyImageView(vk.device.handle, vk.swapchain.image_resources[i].view, nullptr);
            vkFreeCommandBuffers(vk.device.handle, vk.cmd_pool, 1, &vk.swapchain.image_resources[i].cmd);
        }
        vkDestroyCommandPool(vk.device.handle, vk.cmd_pool, nullptr);
        vk.cmd_pool = VK_NULL_HANDLE;
        if (vk.separate_present_queue)
            vkDestroyCommandPool(vk.device.handle, vk.present_cmd_pool, nullptr);
        vk.swapchain.image_resources.clear();
        vkDestroyPipelineLayout(vk.device.handle, vk.pipeline.layout, nullptr);

        // check if formats are incompatible
        if constexpr (false) {
            vkDestroyPipeline(vk.device.handle, vk.pipeline.handle, nullptr);
            vkDestroyPipelineCache(vk.device.handle, vk.pipeline.cache, nullptr);
        }

        prepare();
    }
    void draw(WindowHandle w) {
        vkWaitForFences(vk.device.handle, 1, &vk.per_frame_data[vk.frame_index].fence, VK_TRUE, UINT64_MAX);
        vkResetFences(vk.device.handle, 1, &vk.per_frame_data[vk.frame_index].fence);
        VkResult err;
        do {
            err = vkAcquireNextImageKHR(
                vk.device.handle, vk.swapchain.handle, UINT64_MAX,
                vk.per_frame_data[vk.frame_index].image_acquired_semaphore, VK_NULL_HANDLE,
                &vk.current_buffer);
            if (err == VK_ERROR_OUT_OF_DATE_KHR) {
                resize();
            } else if (err == VK_SUBOPTIMAL_KHR) {
                break;
            } else if (err == VK_ERROR_SURFACE_LOST_KHR) {
                vkDestroySurfaceKHR(vk.instance.handle, vk.surface.handle, nullptr);
                create_surface(w);
                resize();
            } else {
                assert(!err);
            }
        } while (err != VK_SUCCESS);
        VkPipelineStageFlags pipe_stage_flags;
        VkSubmitInfo submit_info;
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = nullptr;
        submit_info.pWaitDstStageMask = &pipe_stage_flags;
        pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &vk.per_frame_data[vk.frame_index].image_acquired_semaphore;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &vk.swapchain.image_resources[vk.current_buffer].cmd;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &vk.per_frame_data[vk.frame_index].draw_complete_semaphore;
        err = vkQueueSubmit(vk.graphics_queue.handle, 1, &submit_info, vk.per_frame_data[vk.frame_index].fence);
        assert(!err);
        if (vk.separate_present_queue) {
            VkFence nullFence = VK_NULL_HANDLE;
            pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            submit_info.waitSemaphoreCount = 1;
            submit_info.pWaitSemaphores = &vk.per_frame_data[vk.frame_index].draw_complete_semaphore;
            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = &vk.swapchain.image_resources[vk.current_buffer].graphics_to_present_cmd;
            submit_info.signalSemaphoreCount = 1;
            submit_info.pSignalSemaphores = &vk.per_frame_data[vk.frame_index].image_ownership_semaphore;
            err = vkQueueSubmit(vk.present_queue.handle, 1, &submit_info, nullFence);
            assert(!err);
        }
        VkPresentInfoKHR present = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores =
                (vk.separate_present_queue)
                    ? &vk.per_frame_data[vk.frame_index].image_ownership_semaphore
                    : &vk.per_frame_data[vk.frame_index].draw_complete_semaphore,
            .swapchainCount = 1,
            .pSwapchains = &vk.swapchain.handle,
            .pImageIndices = &vk.current_buffer,
        };
        err = vkQueuePresentKHR(vk.present_queue.handle, &present);
        vk.frame_index += 1;
        vk.frame_index %= Vk::FRAME_LAG;
        if (err == VK_ERROR_OUT_OF_DATE_KHR) {
            resize();
        } else if (err == VK_SUBOPTIMAL_KHR) {
            VkSurfaceCapabilitiesKHR surfCapabilities;
            err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk.physical_device, vk.surface.handle, &surfCapabilities);
            assert(!err);
            if (surfCapabilities.currentExtent.width != static_cast<uint32_t>(vk.swapchain.size_x) ||
                surfCapabilities.currentExtent.height != static_cast<uint32_t>(vk.swapchain.size_y)) {
                resize();
            }
        } else if (err == VK_ERROR_SURFACE_LOST_KHR) {
            vkDestroySurfaceKHR(vk.instance.handle, vk.surface.handle, nullptr);
            create_surface(w);
            resize();
        } else {
            assert(!err);
        }
    }
};
