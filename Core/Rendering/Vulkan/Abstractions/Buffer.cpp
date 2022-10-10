//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Buffer.h"
#include "../VulkanCore.h"
#include "../VulkanUtilities.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    /* --- SETTER METHODS --- */

    Buffer::Builder &Buffer::Builder::SetMemorySize(const uint64_t givenMemorySize)
    {
        // Save the given memory size
        this->memorySize = givenMemorySize;
        return *this;
    }

    Buffer::Builder &Buffer::Builder::SetMemoryFlags(const VkMemoryPropertyFlags givenMemoryFlags)
    {
        // Save the given memory flags
        this->memoryFlags = givenMemoryFlags;
        return *this;
    }

    Buffer::Builder &Buffer::Builder::SetUsageFlags(const VkBufferUsageFlags givenBufferUsage)
    {
        // Save the given usage flags
        this->bufferUsage = givenBufferUsage;
        return *this;
    }

    void Buffer::CopyFromPointer(void *pointer, uint64_t offset)
    {
        // Create an empty pointer
        void *data;

        // Map memory
        vkMapMemory(VulkanCore::GetLogicalDevice(), vkBufferMemory, offset, memorySize, 0, &data);

        // Copy memory data to Vulkan buffer
        memcpy(pointer, data, memorySize);

        // Unmap the memory
        vkUnmapMemory(VulkanCore::GetLogicalDevice(), vkBufferMemory);
    }

    void Buffer::CopyImage(const Image& givenImage, const glm::vec3 imageOffset, const uint64_t offset)
    {
        // Create a temporary command buffer
        VkCommandBuffer commandBuffer = VulkanUtilities::BeginSingleTimeCommands();

        // Set up image copy region
        VkBufferImageCopy copyRegion{};
        copyRegion.bufferOffset = offset;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;
        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageOffset.x = static_cast<int>(imageOffset.x);
        copyRegion.imageOffset.y = static_cast<int>(imageOffset.y);
        copyRegion.imageOffset.z = static_cast<int>(imageOffset.z);
        copyRegion.imageExtent.width = static_cast<uint32_t>(givenImage.GetWidth());
        copyRegion.imageExtent.height = static_cast<uint32_t>(givenImage.GetHeight());
        copyRegion.imageExtent.depth = static_cast<uint32_t>(givenImage.GetDepth());

        // Copy the image to the buffer
        vkCmdCopyBufferToImage(commandBuffer, this->vkBuffer, givenImage.GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        // Destroy the temporary command buffer
        VulkanUtilities::EndSingleTimeCommands(commandBuffer);
    }

    void Buffer::CopyToBuffer(const Buffer& otherBuffer)
    {
        // Check if the two buffers are compatible
        if (this->memorySize != otherBuffer.memorySize)
        {
            VulkanDebugger::ThrowError("Cannot copy data from one buffer to another with a different memory size!");
        }

        // Create a temporary command buffer
        VkCommandBuffer commandBuffer = VulkanUtilities::BeginSingleTimeCommands();

        // Set up the buffer's copy region
        VkBufferCopy copyRegion{};
        copyRegion.size = this->memorySize;

        // Copy the buffer
        vkCmdCopyBuffer(commandBuffer, vkBuffer, otherBuffer.vkBuffer, 1, &copyRegion);

        // Destroy the temporary command buffer
        VulkanUtilities::EndSingleTimeCommands(commandBuffer);
    }

    /* --- CONSTRUCTORS --- */

    std::unique_ptr<Buffer> Buffer::Builder::Build() const
    {
        // Create and return a new buffer
        return std::make_unique<Buffer>(memorySize, memoryFlags, bufferUsage);
    }


        Buffer::Buffer(const uint64_t givenMemorySize, const VkMemoryPropertyFlags givenMemoryFlags, const VkBufferUsageFlags givenBufferUsage) : memorySize(givenMemorySize), memoryFlags(givenMemoryFlags), bufferUsage(givenBufferUsage)
    {
        // Set up buffer creation info
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = givenMemorySize;
        bufferCreateInfo.usage = givenBufferUsage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Create the Vulkan buffer
        VulkanDebugger::CheckResults(
            vkCreateBuffer(VulkanCore::GetLogicalDevice(), &bufferCreateInfo, nullptr, &vkBuffer),
            "Failed to create buffer with size of [" + std::to_string(givenMemorySize) + "] for [" + std::to_string(givenBufferUsage) + "] usage"
        );

        // Get the Vulkan buffer's memory requirements
        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(VulkanCore::GetLogicalDevice(), vkBuffer, &memoryRequirements);

        // Set up the buffer's memory allocation info
        VkMemoryAllocateInfo memoryAllocationInfo{};
        memoryAllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        memoryAllocationInfo.allocationSize = memoryRequirements.size,
        memoryAllocationInfo.memoryTypeIndex = VulkanUtilities::FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryFlags);

        // Allocate buffer's memory
        VulkanDebugger::CheckResults(
            vkAllocateMemory(VulkanCore::GetLogicalDevice(), &memoryAllocationInfo, nullptr, &vkBufferMemory),
            "Failed to allocate memory for buffer"
        );

        // Bind the allocated memory to the buffer
        vkBindBufferMemory(VulkanCore::GetLogicalDevice(), vkBuffer, vkBufferMemory, 0);
    }

    /* --- DESTRUCTOR --- */

    void Buffer::Destroy()
    {
        vkDestroyBuffer(VulkanCore::GetLogicalDevice(), vkBuffer, nullptr);
        vkFreeMemory(VulkanCore::GetLogicalDevice(), vkBufferMemory, nullptr);
    }
}