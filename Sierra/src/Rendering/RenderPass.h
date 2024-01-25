//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#include "RenderingResource.h"

#include "Image.h"

namespace Sierra
{

    enum class AttachmentLoadOperation : bool
    {
        Clear,
        Load
    };

    enum class AttachmentStoreOperation : bool
    {
        Store,
        Discard
    };

    enum class AttachmentType : bool
    {
        Color,
        Depth
    };
    SR_DEFINE_ENUM_FLAG_OPERATORS(AttachmentType);

    struct RenderPassAttachment
    {
        const std::unique_ptr<Image> &templateImage;
        AttachmentType type = AttachmentType::Color;
        AttachmentLoadOperation loadOperation = AttachmentLoadOperation::Clear;
        AttachmentStoreOperation storeOperation = AttachmentStoreOperation::Store;
    };

    struct SubpassDescription
    {
        const std::initializer_list<uint32> &renderTargets = { };
        const std::initializer_list<uint32> &inputs = { };
    };

    struct RenderPassCreateInfo
    {
        const std::string &name = "Render Pass";
        const std::initializer_list<RenderPassAttachment> &attachments = { };
        const std::initializer_list<SubpassDescription> &subpassDescriptions = { };
    };

    struct RenderPassBeginAttachment
    {
        const std::unique_ptr<Image> &image;
        const Color &clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    };

    class SIERRA_API RenderPass : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Resize(uint32 width, uint32 height) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetAttachmentCount() const { return GetColorAttachmentCount() + HasDepthAttachment(); }

        [[nodiscard]] virtual uint32 GetColorAttachmentCount() const = 0;
        [[nodiscard]] virtual bool HasDepthAttachment() const = 0;

        /* --- OPERATORS --- */
        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~RenderPass() = default;

    protected:
        explicit RenderPass(const RenderPassCreateInfo &createInfo);

    };
}