//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include <vulkan/vulkan.h>
#include <glm/mat4x4.hpp>
#include "../Window.h"
#include "Abstractions/Descriptors.h"
#include "../../../Engine/Components/Lighting/DirectionalLight.h"
#include "../../../Engine/Components/Lighting/PointLight.h"

using Sierra::Core::Rendering::Vulkan::Abstractions::DescriptorPool;
using namespace Sierra::Engine::Components;

/* ! NOTE: Remember to change these in shaders too ! */
#define MAX_MESHES 8192 // Changed as @kael wouldn't stop bitching about it
#define MAX_TEXTURES MAX_MESHES * TOTAL_TEXTURE_TYPES_COUNT
#define MAX_POINT_LIGHTS 64
#define MAX_DIRECTIONAL_LIGHTS 16

struct UniformData
{
    /* Vertex Uniform Data */
    glm::mat4x4 view;
    glm::mat4x4 projection;

    uint32_t directionalLightCount;
    uint32_t pointLightCount;
    float _align1_;
    float _align2_;
};

struct ObjectData
{
    glm::mat4x4 model;
};

struct StorageData
{
    ObjectData objectDatas[MAX_MESHES];
    DirectionalLight::ShaderDirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight::ShaderPointLight pointLights[MAX_POINT_LIGHTS];
};

namespace Sierra::Core::Rendering::Vulkan
{
    using Abstractions::BindlessDescriptorSet;

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
        [[nodiscard]] static inline std::shared_ptr<DescriptorPool>& GetDescriptorPool() { return instance.descriptorPool; }
        [[nodiscard]] static inline std::shared_ptr<BindlessDescriptorSet>& GetGlobalBindlessDescriptorSet() { return instance.globalBindlessDescriptorSet; }
        [[nodiscard]] static inline bool GetDescriptorIndexingSupported()
        {
            #if __APPLE__
                return false;
            #else
                return instance.physicalDeviceFeatures.shaderSampledImageArrayDynamicIndexing;
            #endif
        }

        [[nodiscard]] inline static UniformData* GetUniformDataPtr() { return &instance.uniformData; }
        [[nodiscard]] inline static StorageData* GetStorageDataPtr() { return &instance.storageData; }

        inline static void SetWindow(Window *window) { instance.window = window; }

        inline static void SetLogicalDevice(VkDevice logicalDevice) { instance.logicalDevice = logicalDevice; }
        inline static void SetPhysicalDevice(VkPhysicalDevice physicalDevice) { instance.physicalDevice = physicalDevice; }

        inline static void SetPhysicalDeviceFeatures(VkPhysicalDeviceFeatures physicalDeviceFeatures) { instance.physicalDeviceFeatures = physicalDeviceFeatures; }
        inline static void SetPhysicalDeviceProperties(VkPhysicalDeviceProperties physicalDeviceProperties) { instance.physicalDeviceProperties = physicalDeviceProperties; }
        inline static void SetPhysicalDeviceMemoryProperties(VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties) { instance.physicalDeviceMemoryProperties = physicalDeviceMemoryProperties; }

        inline static void SetSwapchainExtent(VkExtent2D swapchainExtent) { instance.swapchainExtent = swapchainExtent; }
        inline static void SetGraphicsQueue(VkQueue graphicsQueue) { instance.graphicsQueue = graphicsQueue; }
        inline static void SetCommandPool(VkCommandPool commandPool) { instance.commandPool = commandPool; }
        inline static void SetDescriptorPool(std::shared_ptr<DescriptorPool> descriptorPool) { instance.descriptorPool = std::move(descriptorPool); }
        inline static void SetGlobalBindlessDescriptorSet(std::shared_ptr<BindlessDescriptorSet> bindlessDescriptorSet) { instance.globalBindlessDescriptorSet = std::move(bindlessDescriptorSet); }

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
        std::shared_ptr<DescriptorPool> descriptorPool;
        std::shared_ptr<BindlessDescriptorSet> globalBindlessDescriptorSet;

        UniformData uniformData;
        StorageData storageData;
    };

}
