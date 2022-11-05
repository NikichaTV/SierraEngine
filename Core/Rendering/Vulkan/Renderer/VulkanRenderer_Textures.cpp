//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateNullTextures()
    {
        // Create default diffuse texture
        nullDiffuseTexture = Texture::Create({
           .filePath = "Textures/Null/DiffuseNull.jpg",
           .textureType = TEXTURE_TYPE_DIFFUSE,
           .samplerCreateInfo {
               .applyBilinearFiltering = false
           }
        }, true);

        // Create default specular texture
        nullSpecularTexture = Texture::Create({
           .filePath = "Textures/Null/SpecularNull.jpg",
           .textureType = TEXTURE_TYPE_SPECULAR
        }, true);

        // If descriptor indexing supported write default textures to the global descriptor set
        if (VulkanCore::GetDescriptorIndexingSupported())
        {
            globalBindlessDescriptorSet = BindlessDescriptorSet::Build({ BINDLESS_TEXTURE_BINDING }, descriptorPool);
            globalBindlessDescriptorSet->WriteTexture(BINDLESS_TEXTURE_BINDING, nullDiffuseTexture);
            globalBindlessDescriptorSet->WriteTexture(BINDLESS_TEXTURE_BINDING, nullSpecularTexture);
            globalBindlessDescriptorSet->Allocate();

            VulkanCore::SetGlobalBindlessDescriptorSet(globalBindlessDescriptorSet);
        }
    }

}