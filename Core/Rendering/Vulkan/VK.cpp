//
// Created by Nikolay Kanchevski on 19.12.22.
//

#include "VK.h"

#include <GLFW/glfw3.h>
#include <imgui_impl_vulkan.h>

#define VMA_IMPLEMENTATION
#define VMA_STATS_STRING_ENABLED 0
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vk_mem_alloc.h>

#include "../../Version.h"
#include "Abstractions/Texture.h"

#define VK_VERSION VK_API_VERSION_1_1

namespace Sierra::Core::Rendering::Vulkan
{
    VK VK::m_Instance;

    /* --- CONSTRUCTORS --- */

    void VK::Initialize(const VkPhysicalDeviceFeatures givenPhysicalDeviceFeatures)
    {
        m_Instance.requiredPhysicalDeviceFeatures = givenPhysicalDeviceFeatures;

        m_Instance.CreateInstance();

        #if VALIDATION_ENABLED
        m_Instance.CreateDebugMessenger();
        #endif

        m_Instance.CreateDevice();
        m_Instance.CreateVulkanMemoryAllocator();

        m_Instance.CreateCommandPool();
        m_Instance.CreateQueryPool();
        m_Instance.CreateDescriptorPool();

        m_Instance.CreateDefaultTextures();
    }

    /* --- SETTER METHODS --- */

    void VK::CreateInstance()
    {
        using namespace Sierra::Engine;


        // Create application information
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Sierra Engine";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(Version::MAJOR, Version::MINOR, Version::PATCH);
        applicationInfo.pEngineName = "No Engine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(Version::MAJOR, Version::MINOR, Version::PATCH);
        applicationInfo.apiVersion = VK_VERSION;

        // Get GLFW extensions
        uint32_t glfwExtensionCount;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        for (const auto &requiredExtension : requiredInstanceExtensions)
        {
            extensions.push_back(requiredExtension);
        }

        // Check whether all extensions are supported
        ASSERT_WARNING_IF(!ExtensionsSupported(extensions), "Some requested extensions are not supported. They have been automatically disabled, but this could lead to issues");

        // Set up m_Instance creation info
        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.pNext = nullptr;

        // If validation is enabled check validation layers support and bind them to m_Instance
        #if VALIDATION_ENABLED
            ASSERT_ERROR_IF(!ValidationLayersSupported(), "Validation layers requested but are not supported");

            // Set up debug messenger info
            debugMessengerCreateInfo = new VkDebugUtilsMessengerCreateInfoEXT();
            debugMessengerCreateInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugMessengerCreateInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debugMessengerCreateInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debugMessengerCreateInfo->pfnUserCallback = Debugger::ValidationCallback;
            debugMessengerCreateInfo->pUserData = nullptr;

            // Set m_Instance to use the debug messenger
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
            instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) debugMessengerCreateInfo;
        #endif

        // Create m_Instance
        VK_ASSERT(
            vkCreateInstance(&instanceCreateInfo, nullptr, &instance),
            "Could not create Vulkan m_Instance"
        );
    }

    #if VALIDATION_ENABLED
    void VK::CreateDebugMessenger()
    {
        VK_ASSERT(
            CreateDebugUtilsMessengerEXT(instance, debugMessengerCreateInfo, nullptr, &debugMessenger),
            "Failed to create validation messenger"
        );

        delete debugMessengerCreateInfo;
    }
    #endif

    void VK::CreateDevice()
    {
        device = Device::Create({ requiredPhysicalDeviceFeatures });
    }

    void VK::CreateVulkanMemoryAllocator()
    {
        // Get pointers to required Vulkan methods
        VmaVulkanFunctions vulkanFunctions{};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        // Set up VMA creation info
        VmaAllocatorCreateInfo vmaCreteInfo{};
        vmaCreteInfo.instance = instance;
        vmaCreteInfo.physicalDevice = device->GetPhysicalDevice();
        vmaCreteInfo.device = device->GetLogicalDevice();
        vmaCreteInfo.vulkanApiVersion = VK_VERSION;
        vmaCreteInfo.pVulkanFunctions = &vulkanFunctions;

        // Create VMA allocator
        vmaCreateAllocator(&vmaCreteInfo, &vmaAllocator);
    }

    void VK::CreateCommandPool()
    {
        // Set up the command pool creation info
        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = device->GetGraphicsQueueFamily();

        // Create the command pool
        VK_ASSERT(
            vkCreateCommandPool(device->GetLogicalDevice(), &commandPoolCreateInfo, nullptr, &commandPool),
            "Failed to create command pool"
        );
    }

    void VK::CreateQueryPool()
    {
        // Create query pool
        queryPool = QueryPool::Create({ .queryType = VK_QUERY_TYPE_TIMESTAMP });
    }

    void VK::CreateDescriptorPool()
    {
        descriptorPool = DescriptorPool::Builder()
            .SetMaxSets(32768)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
        .Build();
    }

    void VK::CreateDefaultTextures()
    {
        // Create default diffuse texture
        Texture::Create({
            .filePath = "Textures/Null/DiffuseNull.jpg",
            .textureType = TEXTURE_TYPE_DIFFUSE,
            .samplerCreateInfo {
                .applyBilinearFiltering = false
            }
        }, true);

        // Create default specular texture
        Texture::Create({
            .filePath = "Textures/Null/SpecularNull.jpg",
            .textureType = TEXTURE_TYPE_SPECULAR
        }, true);

        // Create default height map texture
        Texture::Create({
            .filePath = "Textures/Null/HeightMapNull.jpg",
            .textureType = TEXTURE_TYPE_HEIGHT_MAP
        }, true);
    }

    /* --- DESTRUCTOR --- */

    void VK::Destroy()
    {
        vkDeviceWaitIdle(m_Instance.device->GetLogicalDevice());

        ImGui_ImplVulkan_Shutdown();

        m_Instance.descriptorPool->Destroy();

        m_Instance.queryPool->Destroy();

        vkDestroyCommandPool(m_Instance.device->GetLogicalDevice(), m_Instance.commandPool, nullptr);

        vmaDestroyAllocator(m_Instance.vmaAllocator);

        m_Instance.device->Destroy();

        #if VALIDATION_ENABLED
        DestroyDebugUtilsMessengerEXT(m_Instance.instance, m_Instance.debugMessenger, nullptr);
        #endif

        vkDestroyInstance(m_Instance.instance, nullptr);
    }

}