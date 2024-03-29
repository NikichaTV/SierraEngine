//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "MetalRenderPass.h"

#include "MetalImage.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalRenderPass::MetalRenderPass(const MetalDevice &device, const RenderPassCreateInfo &createInfo)
        : RenderPass(createInfo), MetalResource(createInfo.name)
    {
        // Save attachment information
        resolvedColorAttachmentIndices.reserve(createInfo.attachments.size());
        for (uint32 i = 0; i < createInfo.attachments.size(); i++)
        {
            const RenderPassAttachment &attachment = *(createInfo.attachments.begin() + i);

            if (attachment.type == RenderPassAttachmentType::Depth)
            {
                depthAttachmentIndex = i;
            }
            else if (attachment.type == RenderPassAttachmentType::Color)
            {
                if (attachment.resolveImage.has_value()) resolvedColorAttachmentIndices.emplace_back(i);
                colorAttachmentCount++;
            }
        }

        // Create a render pass for every subpass (Metal does not have subpasses)
        subpasses.resize(createInfo.subpassDescriptions.size());
        subpassRenderTargets.resize(createInfo.subpassDescriptions.size());
        for (uint32 i = 0; i < createInfo.subpassDescriptions.size(); i++)
        {
            const SubpassDescription &subpassDescription = *(createInfo.subpassDescriptions.begin() + i);

            // Allocate render pass descriptor
            MTLRenderPassDescriptor* &subpass = subpasses[i];
            subpass = [[MTLRenderPassDescriptor alloc] init];
            [subpass setRenderTargetWidth: createInfo.attachments.begin()->templateImage->GetWidth()];
            [subpass setRenderTargetHeight: createInfo.attachments.begin()->templateImage->GetHeight()];

            // Save render targets
            subpassRenderTargets[i] = subpassDescription.renderTargets;

            // Reference render targets
            uint32 subpassColorAttachmentCount = 0;
            for (const auto renderTargetIndex : subpassDescription.renderTargets)
            {
                const RenderPassAttachment &renderTarget = *(createInfo.attachments.begin() + renderTargetIndex);
                SR_ERROR_IF(renderTarget.templateImage->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not use image of attachment [{0}] in subpass [{1}] within render pass [{2}] with a graphics API, which differs from [GraphicsAPI::Metal]!", renderTargetIndex, i, GetName());

                const MetalImage &metalImage = static_cast<MetalImage&>(*renderTarget.templateImage);
                if (renderTarget.type == RenderPassAttachmentType::Color)
                {
                    // Add color attachment
                    MTLRenderPassColorAttachmentDescriptor* const colorAttachment = subpass.colorAttachments[subpassColorAttachmentCount];
                    [colorAttachment setTexture: metalImage.GetMetalTexture()]; // NOTE: We assign texture here, even though it will be overwritten at Begin(), so that pipeline can query its pixel format
                    [colorAttachment setLoadAction: AttachmentLoadOperationToLoadAction(renderTarget.loadOperation)];
                    if (!renderTarget.resolveImage.has_value())
                    {
                        [colorAttachment setStoreAction: AttachmentStoreOperationToStoreAction(renderTarget.storeOperation)];
                        subpassColorAttachmentCount++;
                    }
                    else
                    {
                        SR_ERROR_IF(renderTarget.resolveImage->get()->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not use image [{0}] of attachment [{1}] in render pass [{2}] for resolving, as its graphics API differs from [GraphicsAPI::Metal]!", renderTarget.resolveImage->get()->GetName(), renderTargetIndex, GetName());
                        const MetalImage &metalResolveImage = static_cast<MetalImage&>(*renderTarget.resolveImage->get());

                        [colorAttachment setStoreAction: renderTarget.storeOperation == RenderPassAttachmentStoreOperation::Store ? MTLStoreActionStoreAndMultisampleResolve : MTLStoreActionDontCare];
                        [colorAttachment setResolveTexture: metalResolveImage.GetMetalTexture()];  // NOTE: We assign texture here, even though it will be overwritten at Begin(), so that pipeline can query its pixel format
                    }

                }
                else if (renderTarget.type == RenderPassAttachmentType::Depth)
                {
                    // Set depth attachment
                    MTLRenderPassDepthAttachmentDescriptor* const depthAttachment = subpass.depthAttachment;
                    [depthAttachment setTexture: metalImage.GetMetalTexture()]; // NOTE: We assign texture here, even though it will be overwritten at Begin(), so that pipeline can query its pixel format
                    [depthAttachment setLoadAction: AttachmentLoadOperationToLoadAction(renderTarget.loadOperation)];
                    [depthAttachment setStoreAction: AttachmentStoreOperationToStoreAction(renderTarget.storeOperation)];
                    depthAttachmentIndex = renderTargetIndex;
                }
            }

            // NOTE: Input attachments are not applicable in Metal, so instead a simple regular resource binding is done during command rendering
        }
    }

    /* --- POLLING METHODS --- */

    void MetalRenderPass::Resize(const uint32 width, const uint32 height)
    {
        for (const auto &renderPass : subpasses)
        {
            [renderPass setRenderTargetWidth: width];
            [renderPass setRenderTargetHeight: height];
        }
    }

    /* --- DESTRUCTOR --- */

    MetalRenderPass::~MetalRenderPass()
    {
        for (MTLRenderPassDescriptor* subpass : subpasses)
        {
            [subpass release];
        }
    }

    /* --- CONVERSIONS --- */

    MTLLoadAction MetalRenderPass::AttachmentLoadOperationToLoadAction(const RenderPassAttachmentLoadOperation loadOperation)
    {
        switch (loadOperation)
        {
            case RenderPassAttachmentLoadOperation::Clear:        return MTLLoadActionClear;
            case RenderPassAttachmentLoadOperation::Load:         return MTLLoadActionLoad;
        }

        return MTLLoadActionDontCare;
    }

    MTLStoreAction MetalRenderPass::AttachmentStoreOperationToStoreAction(const RenderPassAttachmentStoreOperation storeOperation)
    {
        switch (storeOperation)
        {
            case RenderPassAttachmentStoreOperation::Store:       return MTLStoreActionStore;
            case RenderPassAttachmentStoreOperation::Discard:     return MTLStoreActionDontCare;
        }

        return MTLStoreActionDontCare;
    }

}
