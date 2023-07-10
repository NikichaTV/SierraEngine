//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Framebuffer.h"

#include "../VK.h"
#include "RenderPass.h"

namespace Sierra::Rendering
{

    /* --- CONSTRUCTORS --- */

    Framebuffer::Framebuffer(const FramebufferCreateInfo &createInfo)
        : width(createInfo.width), height(createInfo.height)
    {
        // Set up the framebuffer creation info
        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = createInfo.renderPass->GetVulkanRenderPass();
        framebufferCreateInfo.attachmentCount = createInfo.attachments.size();
        framebufferCreateInfo.width = createInfo.width;
        framebufferCreateInfo.height = createInfo.height;
        framebufferCreateInfo.layers = 1;

        VkImageView* attachmentsPtr = new VkImageView[createInfo.attachments.size()];
        for (uint i = createInfo.attachments.size(); i--;) attachmentsPtr[i] = createInfo.attachments[i].get()->GetVulkanImageView();
        framebufferCreateInfo.pAttachments = attachmentsPtr;

        // Create the Vulkan framebuffer
        VK_ASSERT(
            vkCreateFramebuffer(VK::GetLogicalDevice(), &framebufferCreateInfo, nullptr, &vkFramebuffer),
            FORMAT_STRING("Failed to create a framebuffer with attachment count of [{0}]", createInfo.attachments.size())
        );

        delete[](attachmentsPtr);
    }

    UniquePtr<Framebuffer> Framebuffer::Create(const FramebufferCreateInfo &createInfo)
    {
        return std::make_unique<Framebuffer>(createInfo);
    }

    /* --- DESTRUCTOR --- */

    void Framebuffer::Destroy()
    {
        // Destroy the Vulkan framebuffer
        vkDestroyFramebuffer(VK::GetLogicalDevice(), vkFramebuffer, nullptr);
    }
}
