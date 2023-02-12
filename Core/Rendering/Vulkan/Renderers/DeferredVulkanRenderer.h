//
// Created by Nikolay Kanchevski on 5.02.23.
//

#pragma once

#include "../../RenderingTemplates.h"
#include "VulkanRenderer.h"
#include "../../../../Engine/Classes/Mesh.h"
#include "../../../../Engine/Components/Lighting/PointLight.h"
#include "../../../../Engine/Components/Lighting/DirectionalLight.h"

using namespace Sierra::Engine::Classes;
using namespace Sierra::Engine::Components;

namespace Sierra::Core::Rendering::Vulkan::Renderers
{
    class DeferredVulkanRenderer : public VulkanRenderer
    {
    private:
        enum RenderedImageValue
        {
            RenderedImage = 0,
            PositionBuffer = 1,
            DiffuseBuffer = 2,
            SpecularBuffer = 3,
            ShininessBuffer = 5,
            NormalBuffer = 6,
            DepthBuffer = 7
        };

        struct MergingRendererPushConstant
        {
            RenderedImageValue renderedImageValue = RenderedImageValue::RenderedImage;

            float _align1_;
            float _align2_;
            float _align3_;
        };

        typedef Pipeline<MeshPushConstant, UniformData, StorageData> ScenePipeline;
        typedef Pipeline<SkyboxPushConstant, UniformData, Abstractions::NullStorageBuffer> SkyboxPipeline;
        typedef Pipeline<MergingRendererPushConstant, NullUniformBuffer, StorageData> MergingPipeline;
    public:
        /* --- CONSTRUCTORS --- */
        DeferredVulkanRenderer(const VulkanRendererCreateInfo &createInfo);
        static UniquePtr<DeferredVulkanRenderer> Create(VulkanRendererCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void Update() override;
        void DrawUI() override;
        void Render() override;

        /* --- SETTER METHODS --- */

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkDescriptorSet GetRenderedTextureDescriptorSet() const override { return renderedImageDescriptorSets[swapchain->GetCurrentFrameIndex()]; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;
    private:
        UniquePtr<Image> positionBuffer;
        UniquePtr<Image> diffuseBuffer;
        UniquePtr<Image> specularAndShininessBuffer;
        UniquePtr<Image> normalBuffer;
        UniquePtr<Image> depthStencilBuffer;
        UniquePtr<Image> renderedImage;

        UniquePtr<RenderPass> deferredRenderPass;
        UniquePtr<Framebuffer> deferredFramebuffer;

        UniquePtr<ScenePipeline> scenePipeline;
        SharedPtr<DescriptorSetLayout> sceneDescriptorSetLayout;

        UniquePtr<MergingPipeline> mergingPipeline;
        UniquePtr<DescriptorSet> mergingDescriptorSet;
        SharedPtr<DescriptorSetLayout> mergingDescriptorSetLayout;

        UniquePtr<Mesh> skyboxMesh;
        UniquePtr<SkyboxPipeline> skyboxPipeline;
        SharedPtr<DescriptorSetLayout> skyboxDescriptorSetLayout;

        UniquePtr<Sampler> textureSampler;
        std::vector<VkDescriptorSet> renderedImageDescriptorSets;
        std::vector<UniquePtr<TimestampQuery>> renderTimestampQueries;

        void InitializeRenderer();
        void CreateSceneRenderingObjects();
        void CreateSkyboxRenderingObjects();
    };

}