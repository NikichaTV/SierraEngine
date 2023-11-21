//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

#include "VulkanResource.h"
#include "../../RenderingContext.h"

#include "VulkanInstance.h"
#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanContext final : public RenderingContext, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit VulkanContext(const RenderingContextCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] const VulkanDevice& GetDevice() const override { return device; };
        [[nodiscard]] inline const VulkanInstance& GetInstance() const { return instance; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        VulkanInstance instance;
        VulkanDevice device;

    };

}