//
// Created by Nikolay Kanchevski on 9.02.23.
//

#pragma once

// * ! NOTE: Remember to change these in shaders too ! * //
#define MAX_MESHES 8192                                  // Changed as @kael wouldn't stop bitching about it
#define MAX_TEXTURES MAX_MESHES * TOTAL_TEXTURE_TYPES_COUNT
#define MAX_POINT_LIGHTS 64
#define MAX_DIRECTIONAL_LIGHTS 16

#define UNIFORM_BUFFER_BINDING 0
#define STORAGE_BUFFER_BINDING 1