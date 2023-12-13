//
// Created by Nikolay Kanchevski on 5.12.23.
//

#include "VulkanBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanBuffer::VulkanBuffer(const VulkanDevice &device, const BufferCreateInfo &createInfo)
        : Buffer(createInfo), VulkanResource(createInfo.name), device(device), data(createInfo.memorySize, 0)
    {
        // Set up buffer create info
        VkBufferCreateInfo bufferCreateInfo = { };
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = createInfo.memorySize;
        bufferCreateInfo.usage = BufferUsageToVkBufferUsageFlags(createInfo.usage);
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Set up buffer allocation info
        VmaAllocationCreateInfo allocationCreateInfo= { };
        allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
        allocationCreateInfo.usage = BufferMemoryLocationToVmaMemoryUsage(createInfo.memoryLocation);
        allocationCreateInfo.memoryTypeBits = std::numeric_limits<uint32>::max();
        allocationCreateInfo.priority = 0.5f;

        // Create and allocate buffer
        const VkResult result = vmaCreateBuffer(device.GetMemoryAllocator(), &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, nullptr);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Failed to create buffer [{0}]! Error code: {1}.", GetName(), result);

        // Map and reset memory
        vmaMapMemory(device.GetMemoryAllocator(), allocation, &data.GetData());
        data.SetMemory(0);
    }

    /* --- POLLING METHODS --- */

    void VulkanBuffer::CopyFromMemory(const void* memoryPointer, const uint64 memorySize, const uint64 sourceOffset, const uint64 destinationOffset)
    {
        data.CopyFromMemory(memoryPointer, memorySize, sourceOffset, destinationOffset);
        vmaFlushAllocation(device.GetMemoryAllocator(), allocation, 0, data.GetMemorySize());
    }

    /* --- DESTRUCTOR --- */

    void VulkanBuffer::Destroy()
    {
        vmaUnmapMemory(device.GetMemoryAllocator(), allocation);
        vmaDestroyBuffer(device.GetMemoryAllocator(), buffer, allocation);
        buffer = VK_NULL_HANDLE;
        allocation = VK_NULL_HANDLE;
    }

    /* --- CONVERSIONS --- */

    VkBufferUsageFlags VulkanBuffer::BufferUsageToVkBufferUsageFlags(const BufferUsage bufferType)
    {
        VkBufferUsageFlags usageFlags = 0;
        if (bufferType & BufferUsage::SourceTransfer)            usageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        if (bufferType & BufferUsage::DestinationTransfer)       usageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        if (bufferType & BufferUsage::Uniform)                   usageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if (bufferType & BufferUsage::Storage)                   usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        if (bufferType & BufferUsage::Index)                     usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (bufferType & BufferUsage::Vertex)                    usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        return usageFlags;
    }

    VmaMemoryUsage VulkanBuffer::BufferMemoryLocationToVmaMemoryUsage(const BufferMemoryLocation memoryLocation)
    {
        switch (memoryLocation)
        {
            case BufferMemoryLocation::Host:        return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
            case BufferMemoryLocation::Device:      return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            case BufferMemoryLocation::Auto:        return VMA_MEMORY_USAGE_AUTO;
            default:                                break;
        }
        
        return VMA_MEMORY_USAGE_AUTO;
    }

}