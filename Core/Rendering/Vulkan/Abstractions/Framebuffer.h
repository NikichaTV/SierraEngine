//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "Image.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    struct FramebufferAttachment
    {
        UniquePtr<Image>& image;
    };

    struct FramebufferCreateInfo
    {
        uint width = 0;
        uint height = 0;
        const std::vector<FramebufferAttachment> &attachments;
        VkRenderPass renderPass = VK_NULL_HANDLE;
    };

    class Framebuffer
    {
    public:
        /* --- CONSTRUCTORS --- */
        Framebuffer(const FramebufferCreateInfo &createInfo);
        static UniquePtr<Framebuffer> Create(FramebufferCreateInfo createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint GetWidth() const { return width; }
        [[nodiscard]] inline uint GetHeight() const { return height; }
        [[nodiscard]] inline VkFramebuffer GetVulkanFramebuffer() const { return this->vkFramebuffer; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Framebuffer);

    private:
        VkFramebuffer vkFramebuffer;
        uint width, height;

    };

}