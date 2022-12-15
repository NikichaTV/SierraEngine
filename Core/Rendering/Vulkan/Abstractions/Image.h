//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include "../../../Debugger.h"
#include "../VulkanTypes.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    struct Dimensions
    {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 1;
    };

    struct ImageCreateInfo
    {
        Dimensions dimensions{};
        ImageFormat format = ImageFormat::UNDEFINED;
        uint32_t mipLevels = 1;

        ImageTiling imageTiling = ImageTiling::OPTIMAL;
        Multisampling sampling = Multisampling::MSAAx1;

        ImageUsage usageFlags = ImageUsage::UNDEFINED;
        MemoryFlags memoryFlags = MemoryFlags::NONE;
    };

    struct SwapchainImageCreateInfo
    {
        VkImage image = VK_NULL_HANDLE;
        ImageFormat format = ImageFormat::UNDEFINED;
        Multisampling sampling = Multisampling::MSAAx1;
        Dimensions dimensions{};
    };

    /// @brief Abstraction to simplify the process of working with Vulkan images and image views.
    class Image
    {
    public:
        /* --- CONSTRUCTORS --- */
        Image(const ImageCreateInfo &createInfo);
        [[nodiscard]] static std::unique_ptr<Image> Create(ImageCreateInfo imageCreateInfo);

        // Only to be used for swapchain images!
        Image(const SwapchainImageCreateInfo &createInfo);
        static std::unique_ptr<Image> CreateSwapchainImage(SwapchainImageCreateInfo swapchainImageCreateInfo);

        /* --- SETTER METHODS --- */
        void CreateImageView(ImageAspectFlags givenAspectFlags);
        void TransitionLayout(ImageLayout newLayout);
        void DestroyImageView();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Dimensions GetDimensions() const
        { return this->dimensions; };

        [[nodiscard]] inline float GetWidth() const
        { return this->dimensions.width; };

        [[nodiscard]] inline float GetHeight() const
        { return this->dimensions.height; };

        [[nodiscard]] inline float GetDepth() const
        { return this->dimensions.depth; };

        [[nodiscard]] inline uint32_t GetMipLevels() const
        { return this->mipLevels; };

        [[nodiscard]] inline ImageFormat GetFormat() const
        { return this->format; };

        [[nodiscard]] inline Multisampling GetSampling() const
        { return this->sampling; };

        [[nodiscard]] inline ImageLayout GetLayout() const
        { return this->layout; };

        [[nodiscard]] inline VkImage GetVulkanImage() const
        { return this->vkImage; };

        [[nodiscard]] inline VkImageView GetVulkanImageView() const
        { ASSERT_WARNING_IF(!imageViewCreated, "Image view not generated. Returning null"); return vkImageView; }

        [[nodiscard]] inline VkDeviceMemory GetVulkanMemory() const
        { return this->vkImageMemory; };

        /* --- DESTRUCTOR --- */
        void Destroy();
        inline ~Image() { Destroy(); }
        Image(const Image &) = delete;
        Image &operator=(const Image &) = delete;

    private:
        Dimensions dimensions{};

        uint32_t mipLevels = 1;
        ImageFormat format = ImageFormat::UNDEFINED;
        Multisampling sampling;
        ImageLayout layout = ImageLayout::UNDEFINED;

        VkImage vkImage = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VkDeviceMemory vkImageMemory = VK_NULL_HANDLE;

        bool imageViewCreated = false;
        bool swapchainImage = false;
    };

}