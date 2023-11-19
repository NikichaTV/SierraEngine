//
// Created by Nikolay Kanchevski on 21.08.23.
//

#pragma once

#include "Engine/Time.h"
#include "Core/Version.h"
#include "Core/WindowManager.h"
#include "Core/PlatformInstance.h"
#include "Rendering/RenderingContext.h"

namespace Sierra
{

    struct ApplicationSettings
    {
        GraphicsAPI graphicsAPI = GraphicsAPI::Auto;
        uint16 maxFrameRate = 0;
    };

    struct ApplicationCreateInfo
    {
        const String &name = "Sierra Application";
        const ApplicationSettings &settings = { };
        Version version = Version({ 1, 0, 0, VersionState::Experimental });
    };

    class SIERRA_API Application
    {
    public:
        /* --- CONSTRUCTORS --- */
        void Run();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const String& GetName() const { return name; }
        [[nodiscard]] inline Version GetVersion() { return version; }

        [[nodiscard]] inline const UniquePtr<PlatformInstance>& GetPlatformInstance() { return platformInstance; }
        [[nodiscard]] inline const UniquePtr<WindowManager>& GetWindowManager() { return windowManager; }

        [[nodiscard]] inline const UniquePtr<RenderingContext>& GetRenderingContext() { return renderingContext; }

        /* --- DESTRUCTOR --- */
        virtual ~Application();

        /* --- OPERATORS --- */
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

    protected:
        explicit Application(const ApplicationCreateInfo &createInfo);

    private:
        virtual void OnStart() = 0;
        virtual bool OnUpdate(const TimeStep &timestep) = 0;

        String name;
        Version version;
        ApplicationSettings settings;

        UniquePtr<PlatformInstance> platformInstance = nullptr;
        UniquePtr<WindowManager> windowManager = nullptr;

        UniquePtr<RenderingContext> renderingContext = nullptr;

        class SIERRA_API FrameLimiter
        {
        public:
            /* --- CONSTRUCTORS --- */
            FrameLimiter() = default;

            /* --- POLLING METHODS --- */
            [[nodiscard]] TimeStep BeginFrame();
            void ThrottleFrame(uint32 targetFrameRate);

        private:
            TimePoint frameStartTime;
            TimePoint lastFrameStartTime;
            TimeStep deltaTime;

        };
        FrameLimiter frameLimiter{};


    };

}
