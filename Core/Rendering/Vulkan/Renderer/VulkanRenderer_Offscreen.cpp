//
// Created by Nikolay Kanchevski on 1.12.22.
//

#include "VulkanRenderer.h"

#include <imgui_impl_vulkan.h>

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateOffscreenRenderer()
    {
        // Create offscreen renderer
        offscreenRenderer = OffscreenRenderer::Create({
            swapchainExtent.width, swapchainExtent.height, maxConcurrentFrames,
            ATTACHMENT_COLOR | ATTACHMENT_DEPTH, (AntiAliasingType) msaaSampleCount
        });

        // Resize the offscreen descriptor sets vector
        offscreenImageDescriptorSets.resize(maxConcurrentFrames);
    }

    void VulkanRenderer::CreateOffscreenImageDescriptorSets()
    {
        // Create a descriptor for each offscreen image
        for (uint32_t i = maxConcurrentFrames; i--;)
        {
            offscreenImageDescriptorSets[i] = ImGui_ImplVulkan_AddTexture(offscreenRenderer->GetSampler()->GetVulkanSampler(), offscreenRenderer->GetColorImage(i)->GetVulkanImageView(), VK_IMAGE_LAYOUT_GENERAL);
        }
    }

}