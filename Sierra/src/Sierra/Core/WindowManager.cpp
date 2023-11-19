//
// Created by Nikolay Kanchevski on 7.11.23.
//

#include "WindowManager.h"

#if SR_PLATFORM_WINDOWS
    #include "Platform/Windows/Win32Window.h"
#elif SR_PLATFORM_LINUX
    #include "Platform/Linux/X11Window.h"
#elif SR_PLATFORM_MACOS
    #include "Platform/MacOS/MacOSInstance.h"
    #include "Platform/MacOS/CocoaWindow.h"
#elif SR_PLATFORM_iOS
    #include "Platform/iOS/iOSInstance.h"
    #include "Platform/iOS/UIKitWindow.h"
#else
    #error "Windowing support for this system has not been implemented!"
#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UniquePtr<WindowManager> WindowManager::Create(const WindowManagerCreateInfo &createInfo)
    {
        return UniquePtr<WindowManager>(new WindowManager(createInfo));
    }

    /* --- POLLING METHODS --- */

    UniquePtr<Window> WindowManager::CreateWindow(const WindowCreateInfo &createInfo) const
    {
        #if SR_PLATFORM_WINDOWS
            #error "Unimplemented!"
        #elif SR_PLATFORM_LINUX
            #error "Unimplemented!"
        #elif SR_PLATFORM_MACOS
            SR_ERROR_IF(platformInstance->GetType() !=+ PlatformType::MacOS, "Cannot create Cocoa window using a platform instance of type [{0}] when it must be [{1}]!", platformInstance->GetType()._to_string(), PlatformType(PlatformType::MacOS)._to_string());
            return std::make_unique<CocoaWindow>(static_cast<const MacOSInstance*>(platformInstance.get())->GetCocoaContext(), createInfo);
        #elif SR_PLATFORM_iOS
            SR_ERROR_IF(platformInstance->GetType() !=+ PlatformType::iOS, "Cannot create UIKit window using a platform instance of type [{0}] when it must be [{1}]!", platformInstance->GetType()._to_string(), PlatformType(PlatformType::iOS)._to_string());
            return std::make_unique<UIKitWindow>(static_cast<const iOSInstance*>(platformInstance.get())->GetUIKitContext(), createInfo);
        #else
            SR_ERROR("Cannot create window on unrecognized operating system!");
            return nullptr;
        #endif
    }

    /* --- PRIVATE METHODS --- */

    WindowManager::WindowManager(const WindowManagerCreateInfo &createInfo)
        : platformInstance(createInfo.platformInstance)
    {

    }

}