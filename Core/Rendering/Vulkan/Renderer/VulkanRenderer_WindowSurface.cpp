//
// Created by Nikolay Kanchevski on 5.10.22.
//

#include "VulkanRenderer.h"
#include "../VulkanCore.h"
#include <GLFW/glfw3.h>

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateWindowSurface()
    {
        // Create GLFW surface
        glfwCreateWindowSurface(this->instance, VulkanCore::GetCoreWindow(), nullptr, &surface);
    }

}