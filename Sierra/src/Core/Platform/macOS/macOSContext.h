//
// Created by Nikolay Kanchevski on 3.09.23.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the macOSContext.h file is only allowed in macOS builds!"
#endif

#include "../../PlatformContext.h"
#include "CocoaContext.h"

namespace Sierra
{

    class SIERRA_API macOSContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit macOSContext(const PlatformContextCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const CocoaContext& GetCocoaContext() const { return cocoaContext; }
        [[nodiscard]] inline PlatformType GetType() const override { return PlatformType::macOS; }

    private:
        CocoaContext cocoaContext;

    };

}