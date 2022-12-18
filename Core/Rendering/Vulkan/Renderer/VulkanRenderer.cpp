//
// Created by Nikolay Kanchevski on 4.10.22.
//

#include "VulkanRenderer.h"

#include <imgui_impl_vulkan.h>

#include "../../../World.h"
#include "../../UI/ImGuiCore.h"
#include "../../../../Engine/Classes/Mesh.h"
#include "../../../../Engine/Components/MeshRenderer.h"

using Rendering::UI::ImGuiCore;
using namespace Sierra::Engine::Classes;
using namespace Sierra::Engine::Components;

namespace Sierra::Core::Rendering::Vulkan
{
    /* --- CONSTRUCTORS --- */
    VulkanRenderer::VulkanRenderer(const std::string &givenTitle, const bool setMaximized, const bool setResizable, const bool setFocusRequirement)
     : window(Window(givenTitle, setMaximized, setResizable, setFocusRequirement))
    {
        Start();
    }

    /* --- POLLING METHODS --- */

    void VulkanRenderer::Start()
    {
        PROFILE_FUNCTION();

        CreateDevice();
        CreateSwapchain();

        CreateOffscreenRenderer();
        CreateRenderPass();
        CreateDescriptorSetLayout();
        CreateGraphicsPipeline();

        CreateFramebuffers();
        CreateCommandBuffers();
        CreateShaderBuffers();

        CreateQueryPool();
        CreateDescriptorPool();
        CreateShaderBuffersDescriptorSets();
        CreateSynchronization();

        CreateNullTextures();

        CreateImGuiInstance();
        CreateOffscreenImageDescriptorSets();

        window.Show();
    }

    void VulkanRenderer::Prepare()
    {
        prepared = true;

        if (!imGuiFrameBegan)
        {
            BeginNewImGuiFrame();
            imGuiFrameBegan = true;
        }

        if (window.IsFocusRequired() && !window.IsFocused()) return;

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
            ASSERT_ERROR("Vulkan renderer is not prepared for rendering properly! Make sure you have called World::Prepare() before calling World::Update()");
        }

        if (window.IsFocusRequired() && !window.IsFocused()) return;

        if (imGuiFrameBegan)
        {
            RenderImGui();
            imGuiFrameBegan = false;
        }

        if (lastSceneViewWidth != ImGuiCore::GetSceneViewWidth() || lastSceneViewHeight != ImGuiCore::GetSceneViewHeight())
        {
            lastSceneViewWidth = ImGuiCore::GetSceneViewWidth();
            lastSceneViewHeight = ImGuiCore::GetSceneViewHeight();
        }

        Draw();

        UpdateRendererInfo();
    }

    void VulkanRenderer::UpdateRendererInfo()
    {
        rendererInfo.verticesDrawn = Mesh::GetTotalVertexCount() + ImGui::GetDrawData()->TotalVtxCount;
        rendererInfo.meshesDrawn = Mesh::GetTotalMeshCount();
    }

    /* --- DESTRUCTOR --- */

    VulkanRenderer::~VulkanRenderer()
    {
        vkDeviceWaitIdle(device->GetLogicalDevice());

        vkDestroyDescriptorPool(device->GetLogicalDevice(), imGuiDescriptorPool, nullptr);

        ImGui_ImplVulkan_Shutdown();

        DestroySwapchainObjects();

        swapchainRenderPass->Destroy();

        vkDestroyQueryPool(device->GetLogicalDevice(), drawTimeQueryPool, nullptr);

        Texture::DestroyDefaultTextures();

        Texture::DisposePool();

        offscreenRenderer->Destroy();

        Shader::DisposePool();

        graphicsPipeline->Destroy();

        for (const auto &buffer : shaderBuffers)
        {
            buffer->Destroy();
        }

        descriptorPool->Destroy();
        descriptorSetLayout->Destroy();

        auto enttMeshView = World::GetEnttRegistry().view<MeshRenderer>();
        for (const auto &entity : enttMeshView)
        {
            enttMeshView.get<MeshRenderer>(entity).Destroy();
        }

        for (uint32_t i = maxConcurrentFrames; i--;)
        {
            vkDestroyFence(device->GetLogicalDevice(), frameBeingRenderedFences[i], nullptr);
            vkDestroySemaphore(device->GetLogicalDevice(), renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device->GetLogicalDevice(), imageAvailableSemaphores[i], nullptr);
        }

        device->Destroy();
    }
}