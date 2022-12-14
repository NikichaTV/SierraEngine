#version 450

#include "../shader_utilities.glsl"

layout(location = 0) in vec3 fromVert_Position;
layout(location = 1) in vec3 fromVert_Normal;
layout(location = 2) in vec2 fromVert_TextureCoordinates;

layout(set = 1, binding = DIFFUSE_TEXTURE_OFFSET + 2) uniform sampler2D diffuseSampler;

layout(location = 0) out vec4 outColor;

void main()
{
        // Read diffuse and specular textures
        vec3 textureColor = texture(diffuseSampler, fromVert_TextureCoordinates).rgb;

        // Submit the final color for drawing
        outColor = vec4(textureColor, 1.0);
}