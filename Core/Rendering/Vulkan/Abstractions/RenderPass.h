//
// Created by Nikolay Kanchevski on 12.12.22.
//

#pragma once

#include "Image.h"
#include "Framebuffer.h"
#include "../VulkanTypes.h"
#include "CommandBuffer.h"

namespace Sierra::Rendering
{

    struct RenderPassAttachment
    {
        /* --- FRAMEBUFFER PROPERTIES --- */
        UniquePtr<Image>& imageAttachment;

        /* --- RENDERPASS PROPERTIES --- */
        LoadOp loadOp =  LoadOp::UNDEFINED;
        StoreOp storeOp = StoreOp::UNDEFINED;
        ImageLayout finalLayout = ImageLayout::UNDEFINED;

        uint resolveTarget = 0;
        bool isResolve = false;

        bool IsDepth() const { return finalLayout == ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL || finalLayout == ImageLayout::DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL || finalLayout == ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL; }
    };

    struct SubpassInfo
    {
        std::vector<uint> renderTargets{};
        std::vector<uint> subpassInputs{};
    };

    struct RenderPassCreateInfo
    {
        const std::vector<RenderPassAttachment> &attachments;
        const std::vector<SubpassInfo> &subpassInfos;
    };

    class RenderPass
    {

    public:
        /* --- CONSTRUCTORS --- */
        explicit RenderPass(const RenderPassCreateInfo &createInfo);
        static UniquePtr<RenderPass> Create(const RenderPassCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        static void NextSubpass(const UniquePtr<CommandBuffer> &commandBuffer);
        void Begin(const UniquePtr<Framebuffer> &framebuffer, const UniquePtr<CommandBuffer> &commandBuffer);
        void End(const UniquePtr<CommandBuffer> &commandBuffer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint GetColorAttachmentCount() const { return colorAttachmentCount; }
        [[nodiscard]] inline bool HasDepthAttachment() const { return hasDepthAttachment; }
        [[nodiscard]] inline VkRenderPass GetVulkanRenderPass() const { return renderPass; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(RenderPass);
//        friend class GraphicsPipeline;

    private:
        VkRenderPass renderPass = VK_NULL_HANDLE;
        std::vector<VkClearValue> clearValues;

        struct SubpassDescription
        {
            VkAttachmentReference depthReference{};
            std::vector<VkAttachmentReference> colorReferences{};
            std::vector<VkAttachmentReference> resolveReferences{};
            std::vector<VkAttachmentReference> inputReferences{};
            VkSubpassDescription data{};
        };

        uint colorAttachmentCount = 0;
        bool hasDepthAttachment = false;
    };

}
