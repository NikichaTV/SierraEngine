//
// Created by Nikolay Kanchevski on 4.10.22.
//

#include <functional>
#include "VulkanRenderer.h"
#include "../../../../Engine/Classes/Stopwatch.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering::Vulkan
{
    /* --- CONSTRUCTORS --- */
    VulkanRenderer::VulkanRenderer(std::string givenTitle, const bool setMaximized, const bool setResizable, const bool setFocusRequirement)
     : window(Window(givenTitle, setMaximized, setResizable, setFocusRequirement))
    {
        Start();
    }

    /* --- POLLING METHODS --- */

    void VulkanRenderer::Start()
    {
        Stopwatch stopwatch;

        CreateInstance();
        if (VALIDATION_ENABLED) CreateValidationMessenger();
        CreateWindowSurface();
        GetPhysicalDevice();
        CreateLogicalDevice();

        CreateCommandPool();
        CreateSwapchain();
        CreateDepthBufferImage();

        CreateRenderPass();
        CreatePushConstants();
        CreateDescriptorSetLayout();
        CreateGraphicsPipeline();

        CreateColorBufferImage();

        CreateFramebuffers();
        CreateTextureSampler();
        CreateCommandBuffers();
        CreateUniformBuffers();

        CreateQueryPool();
        CreateDescriptorPool();
        CreateUniformDescriptorSets();
        CreateSynchronization();

        // CreateNullTextures();
        CreateImGuiInstance();

        auto &mesh = Mesh::RegisterMesh(vertices, meshIndices);

        window.SetResizeCallback([this](void) {
            Draw();
        #if _WIN32
            Draw();
        #endif
        });
        window.Show();
        VulkanDebugger::DisplaySuccess("Successfully started Vulkan! Initialization took: " + std::to_string(stopwatch.GetElapsedMilliseconds()) + "ms");
    }

    void VulkanRenderer::Prepare()
    {
        prepared = true;

        if (window.IsFocusRequired() && !window.IsFocused()) return;

        BeginNewImGuiFrame();
        UpdateImGuiData();
    }

    void VulkanRenderer::UpdateWindow()
    {
        window.Update();
    }

    void VulkanRenderer::Render()
    {
        if (!prepared)
        {
            VulkanDebugger::ThrowError("Vulkan renderer is not prepared for rendering properly! Make sure you have called World::Prepare() before calling World::Update()");
        }

        if (window.IsFocusRequired() && !window.IsFocused()) return;

        RenderImGui();
        Draw();
    }

    /* --- DESTRUCTOR --- */

    VulkanRenderer::~VulkanRenderer()
    {
        vkDeviceWaitIdle(logicalDevice);

        vkDestroyDescriptorPool(logicalDevice, imGuiDescriptorPool, nullptr);
        ImGui_ImplVulkan_Shutdown();

        DestroySwapchainObjects();
        vkDestroySurfaceKHR(instance, surface, nullptr);

        textureSampler->Destroy();

        vkDestroyQueryPool(this->logicalDevice, drawTimeQueryPool, nullptr);

        vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(logicalDevice, graphicsPipelineLayout, nullptr);

        for (const auto &uniformBuffer : uniformBuffers)
        {
            uniformBuffer->Destroy();
        }

        descriptorPool->Destroy();
        descriptorSetLayout->Destroy();

        for (const auto &mesh : Mesh::worldMeshes)
        {
            mesh->Destroy();
        }

        for (int i = MAX_CONCURRENT_FRAMES; i--;)
        {
            vkDestroyFence(logicalDevice, frameBeingRenderedFences[i], nullptr);
            vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
        }

        vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

        vkDestroyDevice(logicalDevice, nullptr);

        if (VALIDATION_ENABLED)
        {
            DestroyDebugUtilsMessengerEXT(instance, validationMessenger, nullptr);
        }

        vkDestroyInstance(instance, nullptr);
    }
}