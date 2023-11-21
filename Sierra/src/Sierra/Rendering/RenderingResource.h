//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

namespace Sierra
{

    #if SR_METAL_SUPPORTED
        #define GRAPHICS_API_AUTO Metal
    #elif SR_VULKAN_SUPPORTED
        #define GRAPHICS_API_AUTO Vulkan
    #elif SR_DIRECTX_SUPPORTED
        #define GRAPHICS_API_AUTO DirectX
    #elif SR_OPENGL_SUPPORTED
        #define GRAPHICS_API_AUTO OpenGL
    #else
        #define GRAPHICS_API_AUTO Undefined
    #endif

    BETTER_ENUM(
        GraphicsAPI, uint8,
        Undefined,
        Vulkan,
        DirectX,
        Metal,
        OpenGL,
        Auto = GRAPHICS_API_AUTO
    );
    #undef GRAPHICS_API_AUTO

    class SIERRA_API RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline virtual GraphicsAPI GetAPI() const = 0;

        /* --- DESTRUCTOR --- */
        virtual void Destroy() { }
        virtual ~RenderingResource() = default;

        /* --- OPERATORS --- */
        RenderingResource(const RenderingResource&) = delete;
        RenderingResource& operator=(const RenderingResource&) = delete;

    protected:
        RenderingResource() = default;

    };

}
