//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include "../../RenderingResource.h"

#if SR_PLATFORM_WINDOWS
    #if !defined(WIN32_LEAN_AND_MEAN)
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <vulkan/vulkan_win32.h>
#elif SR_PLATFORM_macOS
    #include <vulkan/vulkan_macos.h>
#elif SR_PLATFORM_LINUX
    #include <X11/Xlib.h>
    #include <vulkan/vulkan_xlib.h>
#elif SR_PLATFORM_ANDROID
    #include <vulkan/vulkan_android.h>
#elif SR_PLATFORM_iOS
    #include <vulkan/vulkan_ios.h>
#endif


namespace Sierra
{

    class SIERRA_API VulkanResource : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline GraphicsAPI GetAPI() const override { return GraphicsAPI::Vulkan; };

    protected:
        inline explicit VulkanResource(const std::string &name)
        {
            #if SR_ENABLE_LOGGING
                this->name = name;
            #endif
        }
        static void PushToPNextChain(void* mainStruct, void* newStruct);

    };

}
