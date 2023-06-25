//
// Created by Nikolay Kanchevski on 19.06.23.
//

#pragma once

#include "Pipeline.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    struct ComputePipelineCreateInfo
    {
        SharedPtr<Shader> &shader;
        Optional<CompiledPipelineShaderInfo> shaderInfo = std::nullopt;
    };

    class ComputePipeline : public Pipeline
    {
    public:
        /* --- CONSTRUCTORS --- */
        ComputePipeline(const ComputePipelineCreateInfo &createInfo);
        static UniquePtr<ComputePipeline> Create(const ComputePipelineCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Dispatch(const UniquePtr<CommandBuffer> &commandBuffer, uint xCount, uint yCount = 1, uint zCount = 1);

        /* --- DESTRUCTOR --- */
        void Destroy() override;
        DELETE_COPY(ComputePipeline);

    private:
        void CreatePipeline() override;

    };

}