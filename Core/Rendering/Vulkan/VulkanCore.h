//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include <vulkan/vulkan.h>
#include "../Window.h"

namespace Sierra::Core::Rendering::Vulkan
{

    class VulkanCore
    {
    public:
        [[nodiscard]] static inline VulkanCore& GetInstance() { return instance; }

        [[nodiscard]] static inline Window* GetWindow() { return GetInstance().window; }
        [[nodiscard]] static inline GLFWwindow* GetCoreWindow() { return GetInstance().window->GetCoreWindow(); }

        [[nodiscard]] static inline VkDevice& GetLogicalDevice() { return GetInstance().logicalDevice; }
        [[nodiscard]] static inline VkPhysicalDevice& GetPhysicalDevice() { return GetInstance().physicalDevice; }

        [[nodiscard]] static inline VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() { return GetInstance().physicalDeviceFeatures; };
        [[nodiscard]] static inline VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() { return GetInstance().physicalDeviceProperties; };
        [[nodiscard]] static inline VkPhysicalDeviceMemoryProperties& GetPhysicalDeviceMemoryProperties() { return GetInstance().physicalDeviceMemoryProperties; };

        [[nodiscard]] static inline VkExtent2D& GetSwapchainExtent() { return instance.swapchainExtent; }
        [[nodiscard]] static inline VkQueue& GetGraphicsQueue() { return instance.graphicsQueue; }
        [[nodiscard]] static inline VkCommandPool& GetCommandPool() { return instance.commandPool; }

        inline static void SetWindow(Window *window) { instance.window = window; }

        inline static void SetLogicalDevice(VkDevice logicalDevice) { instance.logicalDevice = logicalDevice; }
        inline static void SetPhysicalDevice(VkPhysicalDevice physicalDevice) { instance.physicalDevice = physicalDevice; }

        inline static void SetPhysicalDeviceFeatures(VkPhysicalDeviceFeatures physicalDeviceFeatures) { instance.physicalDeviceFeatures = physicalDeviceFeatures; }
        inline static void SetPhysicalDeviceProperties(VkPhysicalDeviceProperties physicalDeviceProperties) { instance.physicalDeviceProperties = physicalDeviceProperties; }
        inline static void SetPhysicalDeviceMemoryProperties(VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties) { instance.physicalDeviceMemoryProperties = physicalDeviceMemoryProperties; }

        inline static void SetSwapchainExtent(VkExtent2D swapchainExtent) { instance.swapchainExtent = swapchainExtent; }
        inline static void SetGraphicsQueue(VkQueue graphicsQueue) { instance.graphicsQueue = graphicsQueue; }
        inline static void SetCommandPool(VkCommandPool commandPool) { instance.commandPool = commandPool; }

    private:
        VulkanCore() = default;
        static VulkanCore instance;

        Window *window;
        VkDevice logicalDevice;
        VkPhysicalDevice physicalDevice;

        VkPhysicalDeviceFeatures physicalDeviceFeatures;
        VkPhysicalDeviceProperties physicalDeviceProperties;
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

        VkExtent2D swapchainExtent;
        VkQueue graphicsQueue;
        VkCommandPool commandPool;
    };

}
