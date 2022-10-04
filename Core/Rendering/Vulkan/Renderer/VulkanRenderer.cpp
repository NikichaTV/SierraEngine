//
// Created by Nikolay Kanchevski on 4.10.22.
//

#include "VulkanRenderer.h"
#include "../../../../Engine/Classes/Stopwatch.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering::Vulkan
{

    /* --- POLLING METHODS --- */

    void VulkanRenderer::Start()
    {
        Stopwatch stopwatch;

        CreateInstance();

        VulkanDebugger::DisplayInfo("Successfully started Vulkan! Initialization took " + std::to_string(stopwatch.GetElapsedMilliseconds()) + "ms");
    }

    void VulkanRenderer::Update()
    {

    }

    /* --- DESTRUCTOR --- */

    VulkanRenderer::~VulkanRenderer()
    {

    }
}