//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "../../CommandBuffer.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanCommandBuffer final : public CommandBuffer, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanCommandBuffer(const VulkanDevice &device, const CommandBufferCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Begin() override;
        void End() override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkCommandBuffer GetVulkanCommandBuffer() const { return commandBuffer; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        const VulkanDevice &device;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

    };

}