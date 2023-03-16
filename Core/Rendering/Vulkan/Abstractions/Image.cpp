//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Image.h"

#include "../VK.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    Image::Image(const ImageCreateInfo &createInfo)
        : dimensions(createInfo.dimensions), layerCount(createInfo.layerCount), usage(createInfo.usage), sampling(createInfo.sampling), format(createInfo.format)
    {
        // Set up image creation info
        VkImageCreateInfo vkImageCreateInfo{};
        vkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        vkImageCreateInfo.imageType = (VkImageType) createInfo.imageType;

        vkImageCreateInfo.extent.width = static_cast<uint>(dimensions.width);
        vkImageCreateInfo.extent.height = static_cast<uint>(dimensions.height);
        vkImageCreateInfo.extent.depth = static_cast<uint>(dimensions.depth);

        if (createInfo.generateMipMaps) mipLevels = static_cast<uint>(std::floor(std::log2(std::max(dimensions.width, dimensions.height)))) + 1;

        vkImageCreateInfo.mipLevels = mipLevels;
        vkImageCreateInfo.arrayLayers = createInfo.layerCount;
        vkImageCreateInfo.format = (VkFormat) format;
        vkImageCreateInfo.tiling = (VkImageTiling) createInfo.imageTiling;
        vkImageCreateInfo.initialLayout = (VkImageLayout) ImageLayout::UNDEFINED;
        vkImageCreateInfo.usage = (VkImageUsageFlagBits) createInfo.usage;
        vkImageCreateInfo.samples = (VkSampleCountFlagBits) sampling;
        vkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vkImageCreateInfo.flags = static_cast<uint>(createInfo.createFlags);

        // Set up image allocation info
        VmaAllocationCreateInfo allocationCreateInfo{};
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocationCreateInfo.memoryTypeBits = UINT_MAX;
        allocationCreateInfo.priority = 0.5f;

        // Create and allocate image
        VK_ASSERT(
            vmaCreateImage(VK::GetMemoryAllocator(), &vkImageCreateInfo, &allocationCreateInfo, &vkImage, &vmaImageAllocation, nullptr),
            FORMAT_STRING("Failed to allocate memory for image with dimensions of [{0}, {1}, {2}], format [{3}], [{4}] mip levels, and sampling of [{5}]", dimensions.width, dimensions.height, dimensions.depth, VK_TO_STRING(format, Format), mipLevels, VK_TO_STRING(sampling, SampleCountFlagBits))
        );
    }

    UniquePtr<Image> Image::Create(ImageCreateInfo imageCreateInfo)
    {
        return std::make_unique<Image>(imageCreateInfo);
    }

    Image::Image(const SwapchainImageCreateInfo &createInfo)
        : vkImage(createInfo.image), format(createInfo.format), usage(ImageUsage::UNDEFINED), sampling(createInfo.sampling), dimensions(createInfo.dimensions), layout(ImageLayout::UNDEFINED), swapchainImage(true)
    {

    }

    UniquePtr<Image> Image::CreateSwapchainImage(SwapchainImageCreateInfo swapchainImageCreateInfo)
    {
        return std::make_unique<Image>(swapchainImageCreateInfo);
    }

    /* --- SETTER METHODS --- */

    bool Image::GenerateMipMaps()
    {
        if (dimensions.width * dimensions.height < 65536) // 256 * 256
        {
            ASSERT_WARNING_FORMATTED("Image is too small for mip map generation. Its size is {0}x{1}, while an image of total size bigger than 512x512 is required. Action suspended automatically", GetWidth(), GetHeight());
            return false;
        }

        if (layout != ImageLayout::TRANSFER_DST_OPTIMAL)
        {
            ASSERT_WARNING("Image's layout must be [TRANSFER_DST_OPTIMAL] in order to generate mip maps for it! Action was automatically suspended");
            return false;
        }

        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(VK::GetPhysicalDevice(), (VkFormat) format, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            ASSERT_WARNING_FORMATTED("Generating mip maps for images with format of [{0}] is not supported! Action was automatically suspended", static_cast<int>(format));
            return false;
        }

        auto commandBuffer = VK::GetDevice()->BeginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = vkImage;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = layerCount;
        barrier.subresourceRange.levelCount = 1;

        int mipWidth = dimensions.width;
        int mipHeight = dimensions.height;

        for (uint i = 1; i < mipLevels; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer->GetVulkanCommandBuffer(),
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer->GetVulkanCommandBuffer(),
                vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR
            );

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer->GetVulkanCommandBuffer(),
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer->GetVulkanCommandBuffer(),
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        VK::GetDevice()->EndSingleTimeCommands(commandBuffer);

        // Layout has been transitioned automatically
        layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

        mipMapsGenerated = true;
        return true;
    }

    void Image::CreateImageView(const ImageAspectFlags givenAspectFlags, const VkImageViewType imageViewType)
    {
        // Check if an image view has already been generated
        if (imageViewCreated)
        {
            ASSERT_WARNING("Trying to create an image view for an image with an already existing view. Process automatically suspended");
            return;
        }

        // Set up image view creation info
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = vkImage;
        imageViewCreateInfo.viewType = imageViewType;
        imageViewCreateInfo.format = (VkFormat) format;
        imageViewCreateInfo.subresourceRange.aspectMask = (VkImageAspectFlagBits) givenAspectFlags;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = layerCount;

        // Create the image view
        VK_ASSERT(
            vkCreateImageView(VK::GetLogicalDevice(), &imageViewCreateInfo, nullptr, &vkImageView),
            FORMAT_STRING("Failed to create image view for an image with dimensions of [{0}, {1}, {2}], format [{3}], [{4}] mip levels, and sampling of [{5}]", dimensions.width, dimensions.height, dimensions.depth, static_cast<int>(format), static_cast<int>(mipLevels), static_cast<int>(sampling))
        );

        aspectFlags = givenAspectFlags;

        imageViewCreated = true;
    }

    void Image::TransitionLayout(const ImageLayout newLayout)
    {
        // Create a temporary command buffer
        auto commandBuffer = VK::GetDevice()->BeginSingleTimeCommands();

        // Set up shader stages
        VkPipelineStageFlags srcStage = 0;
        VkPipelineStageFlags dstStage = 0;

        // If transitioning from a new or undefined image to an image that is ready to receive data...
        if (layout == ImageLayout::UNDEFINED && newLayout == ImageLayout::TRANSFER_DST_OPTIMAL)
        {
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;  // The stage the transition must occur after
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;     // The stage the transition must occur before
        }
        // If transitioning from transfer destination to shader readable...
        else if (layout == ImageLayout::TRANSFER_DST_OPTIMAL && newLayout == ImageLayout::SHADER_READ_ONLY_OPTIMAL)
        {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        // If transitioning from an undefined layout to one optimal for depth stencil...
        else if (layout == ImageLayout::UNDEFINED && newLayout == ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL)
        {
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            ASSERT_ERROR_FORMATTED("Transitioning images from [{0}] to [{1}] is not supported", static_cast<uint>(layout), static_cast<uint>(newLayout));
        }

        // Record transition command
        commandBuffer->TransitionImageLayout(this, newLayout, srcStage, dstStage);

        // End command buffer
        VK::GetDevice()->EndSingleTimeCommands(commandBuffer);
    }

    void Image::DestroyImageView()
    {
        vkDestroyImageView(VK::GetLogicalDevice(), this->vkImageView, nullptr);
        imageViewCreated = false;
    }

    /* --- DESTRUCTOR --- */

    void Image::Destroy()
    {
        if (vkImage == VK_NULL_HANDLE) return;

        if (!swapchainImage)
        {
            vmaDestroyImage(VK::GetMemoryAllocator(), vkImage, vmaImageAllocation);
        }

        if (imageViewCreated) vkDestroyImageView(VK::GetLogicalDevice(), this->vkImageView, nullptr);

        vkImage = VK_NULL_HANDLE;
    }
}