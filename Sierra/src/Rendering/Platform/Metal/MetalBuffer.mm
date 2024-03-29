//
// Created by Nikolay Kanchevski on 5.12.23.
//

#include "MetalBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalBuffer::MetalBuffer(const MetalDevice &device, const BufferCreateInfo &createInfo)
        : Buffer(createInfo), MetalResource(createInfo.name)
    {
        // Create buffer
        buffer = [device.GetMetalDevice() newBufferWithLength: createInfo.memorySize options: BufferMemoryLocationToResourceOptions(createInfo.memoryLocation)];
        SR_ERROR_IF(buffer == nil, "[Metal]: Failed to create buffer [{0}]!", GetName());
        device.SetResourceName(buffer, GetName());

        // Map and reset memory if CPU-visible
        if (createInfo.memoryLocation == BufferMemoryLocation::CPU) std::memset([buffer contents], 0, createInfo.memorySize);
    }

    /* --- POLLING METHODS --- */

    void MetalBuffer::CopyFromMemory(const void* memoryPointer, uint64 memoryRange, const uint64 sourceByteOffset, const uint64 destinationByteOffset)
    {
        memoryRange = memoryRange != 0 ? memoryRange : GetMemorySize();
        SR_ERROR_IF(destinationByteOffset + memoryRange > GetMemorySize(), "[Metal]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, to buffer [{2}], as the resulting memory space of a total of [{3}] bytes is bigger than the size of the buffer - [{4}]!", memoryRange, destinationByteOffset, GetName(), destinationByteOffset + memoryRange, GetMemorySize());
        std::memcpy(reinterpret_cast<char*>([buffer contents]) + destinationByteOffset, reinterpret_cast<const char*>(memoryPointer) + sourceByteOffset, memoryRange);
    }

    /* --- DESTRUCTOR --- */

    MetalBuffer::~MetalBuffer()
    {
        [buffer release];
    }

    /* --- CONVERSIONS --- */

    MTLResourceOptions MetalBuffer::BufferMemoryLocationToResourceOptions(const BufferMemoryLocation memoryLocation)
    {
        switch (memoryLocation)
        {
            case BufferMemoryLocation::CPU:      return MTLResourceStorageModeShared | MTLResourceHazardTrackingModeUntracked | MTLResourceCPUCacheModeDefaultCache;
            case BufferMemoryLocation::GPU:      return MTLResourceStorageModePrivate | MTLResourceHazardTrackingModeUntracked | MTLResourceCPUCacheModeWriteCombined;
        }

        return MTLResourceStorageModeShared | MTLResourceHazardTrackingModeUntracked | MTLResourceCPUCacheModeDefaultCache;
    }

}
