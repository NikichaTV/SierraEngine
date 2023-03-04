//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include "../../Engine/Include.h"

class Application
{
public:
    /* --- POLLING METHODS --- */
    void Start();

private:
    const float CAMERA_MOVE_SPEED = 15.0f, CAMERA_LOOK_SPEED = 0.1f, GAMEPAD_CAMERA_LOOK_SPEED = 0.5f;

    Camera camera;
    PointLight* pointLight = nullptr;
    std::vector<UniquePtr<Model>> tankModels;

    void RenderLoop(UniquePtr<VulkanRenderer> &renderer);

    void UpdateObjects();
    void DoCameraMovement();
};