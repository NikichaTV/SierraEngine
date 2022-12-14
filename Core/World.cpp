//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "World.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../Engine/Classes/Time.h"
#include "../Engine/Classes/Cursor.h"
#include "../Engine/Classes/Input.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Core
{
    /* --- POLLING METHODS --- */

    void World::Start()
    {
        Input::Start();
        Cursor::Start();
    }

    void World::Prepare()
    {
        Time::Update();
        Cursor::Update();
        Input::Update();
    }

    void World::Update()
    {
        UpdateObjects();
    }

    /* --- SETTER METHODS --- */

    void World::UpdateObjects()
    {
//        // Update camera
//        Camera *camera = Camera::GetMainCamera();
//        Transform &cameraTransform = enttRegistry->get<Transform>(camera->GetEnttEntity());
//        glm::vec3 rendererCameraPosition = { cameraTransform.position.x, -cameraTransform.position.y, cameraTransform.position.z };
//        glm::vec3 rendererCameraFrontDirection = { camera->GetFrontDirection().x, -camera->GetFrontDirection().y, camera->GetFrontDirection().z };
//        glm::vec3 rendererCameraUpDirection = { camera->GetUpDirection().x, -camera->GetUpDirection().y, camera->GetUpDirection().z };
//
//        // Update uniform data
//        auto &uniformData = VulkanCore::GetUniformDataPtr();
//        uniformData.view = glm::lookAt(rendererCameraPosition, rendererCameraPosition + rendererCameraFrontDirection, rendererCameraUpDirection);
//        uniformData.projection = glm::perspective(glm::radians(camera->fov), (float) ImGuiCore::GetSceneViewWidth() / (float) ImGuiCore::GetSceneViewHeight(), camera->nearClip, camera->farClip);
//        uniformData.projection[1][1] *= -1;
//        uniformData.directionalLightCount = DirectionalLight::GetDirectionalLightCount();
//        uniformData.pointLightCount = PointLight::GetPointLightCount();
//
//        // Update storage data
//        auto &storageData = VulkanCore::GetStorageDataPtr();
//        auto enttMeshView = World::GetEnttRegistry()->view<MeshRenderer>();
//        for (auto enttEntity : enttMeshView)
//        {
//            MeshRenderer &meshRenderer = enttMeshView.get<MeshRenderer>(enttEntity);
//            storageData.objectDatas[meshRenderer.GetMeshID()].model = meshRenderer.GetModelMatrix();
//        }
//
//        auto enttDirectionalLightView = World::GetEnttRegistry()->view<DirectionalLight>();
//        for (auto enttEntity : enttDirectionalLightView)
//        {
//            DirectionalLight &directionalLight = enttDirectionalLightView.get<DirectionalLight>(enttEntity);
//            storageData.directionalLights[directionalLight.GetID()] = directionalLight;
//        }
//
//        auto enntPointLightView = World::GetEnttRegistry()->view<PointLight>();
//        for (auto enttEntity : enntPointLightView)
//        {
//            PointLight &pointLight = enntPointLightView.get<PointLight>(enttEntity);
//            storageData.pointLights[pointLight.GetID()] = pointLight;
//        }

    }

}
