//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Descriptors.h"
#include "Sampler.h"
#include "Image.h"

// TODO: Move this to VulkanRenderer_Textures
enum TextureType { None = 0, Diffuse = 1, Specular = 2, Normal = 3, Height = 4 };

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    class Texture
    {
    public:
        /* --- CONSTRUCTORS --- */
        Texture(const stbi_uc *stbImage, uint32_t width, uint32_t height, TextureType givenTextureType, int givenColorChannelsCount, unsigned long givenMemorySize, bool givenMipMappingEnabled, VkSampler givenSampler,
                DescriptorSetLayout &givenDescriptorSetLayout, DescriptorPool &givenDescriptorPool, std::string givenName);

        class Builder
        {
        public:
            Builder(DescriptorSetLayout &givenDescriptorSetLayout, DescriptorPool &givenDescriptorPool);
            Builder& SetName(std::string givenName);
            Builder& SetTextureType(TextureType givenTextureType);
            Builder& EnableMipMapGeneration(bool isApplied);
            Builder& SetSampler(Sampler &givenSampler);
            [[nodiscard]] std::unique_ptr<Texture> Build(std::string filePath);

        private:
            std::string name = "";
            TextureType textureType = TextureType::None;
            bool mipMappingEnabled = true;
            DescriptorSetLayout &descriptorSetLayout;
            DescriptorPool &descriptorPool;
            VkSampler vkSampler;
            unsigned long imageSize;
        };

        /* --- SETTER METHODS --- */

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline std::string GetName() const
        { return this->name; }

        [[nodiscard]] inline uint32_t GetWidth() const
        { return this->image->GetWidth(); }

        [[nodiscard]] inline uint32_t GetHeight() const
        { return this->image->GetHeight(); }

        [[nodiscard]] inline uint32_t GetDepth() const
        { return this->image->GetDepth(); }

        [[nodiscard]] inline VkFormat GetImageFormat() const
        { return this->image->GetFormat(); }

        [[nodiscard]] inline TextureType GetTextureType() const
        { return this->textureType; }

        [[nodiscard]] inline uint32_t GetMipMapLevels() const
        { return this->mipMapLevels; }

        [[nodiscard]] inline bool GetMipMappingEnabled() const
        { return this->mipMappingEnabled; }

        [[nodiscard]] inline unsigned long GetMemorySize() const
        { return this->memorySize; }

        [[nodiscard]] inline VkDescriptorSet GetDescriptorSet() const
        { return this->descriptorSet; }

        [[nodiscard]] inline Image& GetImage() const
        { return *this->image; }

        /* --- SETTER METHODS --- */

        /* --- DESTRUCTOR --- */
        // NOTE: No destructor as the image will automatically be destroyed
        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;
    private:
        std::string name;
        TextureType textureType;
        uint32_t colorChannelsCount;

        VkSampler sampler;
        uint32_t mipMapLevels = 1;
        bool mipMappingEnabled;

        unsigned long memorySize;
        VkDescriptorSet descriptorSet;

        std::unique_ptr<Image> image;
        void GenerateMipMaps();
    };

}