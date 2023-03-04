//
// Created by Nikolay Kanchevski on 17.02.23.
//

#include "UIPanels.h"

#include "../ImGuiCore.h"
#include "../../../EngineCore.h"
#include "../../Math/MatrixUtilities.h"
#include "../../../../Engine/Classes/Input.h"
#include "../../../../Engine/Classes/Math.h"
#include "../../../../Engine/Classes/Time.h"
#include "../../../../Engine/Classes/Mesh.h"
#include "../../../../Engine/Components/Camera.h"
#include "../../Vulkan/Renderers/VulkanRenderer.h"
#include "../../../../Engine/Components/Relationship.h"
#include "../../../../Engine/Components/MeshRenderer.h"

using namespace Sierra::Engine::Classes;
using namespace Sierra::Engine::Components;

namespace Sierra::Core::Rendering::UI
{
    void ViewportPanel::DrawUI()
    {
        // Set up dock space and window flags
        ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground |
                                       ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // Get a pointer to the main viewport of ImGui
        const ImGuiViewport *viewport = ImGui::GetMainViewport();

        // Set window sizing properties accordingly
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        // Get the docking of the main viewport
        ImGuiID dockSpaceID = ImGui::GetID("ViewportDock");

        // Disable window padding
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        // Create main viewport window
        GUI::BeginWindow("Viewport", nullptr, windowFlags);

        // Use dock space
        ImGui::DockSpace(dockSpaceID, ImVec2(0.0f, 0.0f), dockSpaceFlags);

        // Create menu bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Preferences"))
            {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("I wanna die"))
            {
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // Remove the overridden window padding
        ImGui::PopStyleVar(1);

        // Finalize main viewport window
        GUI::EndWindow();
    }

    void ListDeeper(Relationship &relationship, const uint iteration)
    {
        ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;

        bool selected = false;
        if (!EngineCore::GetSelectedEntity().IsNull() && EngineCore::GetSelectedEntity().GetComponent<UUID>() == relationship.GetComponent<UUID>())
        {
            selected = true;
            treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
            ImGui::PushStyleColor(ImGuiCol_Header, { 0.11f, 0.32f, 0.5f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 0.1f, 0.26f, 0.45f, 1.0f });
        }

        bool opened = ImGui::TreeNodeEx((void*) relationship.GetComponent<UUID>().GetValue(), treeNodeFlags, "%s", relationship.GetComponent<Tag>().tag.c_str());

        if (ImGui::IsItemClicked())
        {
            EngineCore::SetSelectedEntity(Entity(relationship.GetEnttEntity()));
        }

        if (selected)
        {
            ImGui::PopStyleColor(2);
        }

        if (opened)
        {
            for (auto child : relationship.GetEnttChildrenEntities())
            {
                Relationship &childRelationship = World::GetComponent<Relationship>(child);
                ListDeeper(childRelationship, iteration + 1);
            }

            ImGui::TreePop();
        }


        if (iteration == 0) ImGui::Separator();
    }

    void HierarchyPanel::DrawUI()
    {
        // Create hierarchy tab
        if (GUI::BeginWindow("Hierarchy", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_HorizontalScrollbar))
        {
            ImGui::Separator();

            // Recursively show all entities in the hierarchy
            for (const auto &entityData : World::GetOriginEntitiesList())
            {
                Relationship &entityRelationship = World::GetComponent<Relationship>(entityData.second);
                if (entityRelationship.GetEnttParentEntity() == entt::null)
                {
                    ListDeeper(entityRelationship, 0);
                }
            }

            // Deselect object on click
            if (Input::GetMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && ImGui::IsWindowHovered())
            {
                EngineCore::SetSelectedEntity(Entity::Null);
            }

            GUI::EndWindow();
        }
    }

    void PropertiesPanel::DrawUI()
    {
        if (GUI::BeginWindow("Properties", nullptr, ImGuiWindowFlags_NoNav))
        {
            if (!EngineCore::GetSelectedEntity().IsNull())
            {
                Entity &selectedEntity = EngineCore::GetSelectedEntity();

                GUI::DrawComponent<UUID>(selectedEntity);
                GUI::DrawComponent<Tag>(selectedEntity);
                GUI::DrawComponent<Transform>(selectedEntity);
                GUI::DrawComponent<MeshRenderer>(selectedEntity);
                GUI::DrawComponent<Camera>(selectedEntity);
                GUI::DrawComponent<DirectionalLight>(selectedEntity);
                GUI::DrawComponent<PointLight>(selectedEntity);
            }

            GUI::EndWindow();
        }
    }

    void RendererViewportPanel::DrawUI()
    {
        // Create scene view tab
        ImDrawList *sceneDrawList;
        if (GUI::BeginWindow("Scene View", nullptr))
        {
            sceneDrawList = ImGui::GetWindowDrawList();

            // Get and show current renderer images
            ImVec2 freeSpace = ImGui::GetContentRegionAvail();
            ImGuiCore::SetSceneViewSize(freeSpace.x, freeSpace.y);
            ImGuiCore::SetSceneViewPosition(ImGui::GetCurrentWindow()->WorkRect.GetTL().x, ImGui::GetCurrentWindow()->WorkRect.GetTL().y);

            // Flip renderer image
            if (renderer.GetRenderedTextureDescriptorSet()) ImGui::Image((ImTextureID) renderer.GetRenderedTextureDescriptorSet(), freeSpace, { 0.0f, 1.0f }, { 1.0f, 0.0f });
            GUI::EndWindow();
        }

        if (!renderer.GetImGuiInstance()->HasImGuizmoLayer()) return;

        // Prepare ImGuizmo
        ImGuizmo::Enable(true);
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetRect(ImGuiCore::GetSceneViewPositionX(), ImGuiCore::GetSceneViewPositionY(), ImGuiCore::GetSceneViewWidth(), ImGuiCore::GetSceneViewHeight());
        ImGuizmo::SetDrawlist(sceneDrawList);

        // Set gizmo mode
        static ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
        if (Input::GetKeyPressed(GLFW_KEY_C)) operation = ImGuizmo::TRANSLATE | ImGuizmo::ROTATE | ImGuizmo::SCALE;
        if (Input::GetKeyPressed(GLFW_KEY_R)) operation = ImGuizmo::SCALE;
        if (Input::GetKeyPressed(GLFW_KEY_E)) operation = ImGuizmo::ROTATE;
        if (Input::GetKeyPressed(GLFW_KEY_W)) operation = ImGuizmo::TRANSLATE;

        // Set the renderer's context
        ImGuizmo::SetImGuiContext(renderer.GetImGuiInstance()->GetImGuiContext());

        // Convert camera's view matrix to array data
        Camera &camera = Camera::GetMainCamera();
        camera.CalculateProjectionMatrices();
        Matrix4x4 viewMatrix = camera.GetViewMatrix();
        Matrix4x4 projectionMatrix = camera.GetProjectionMatrix();

        // Show cube view
        ImGuizmo::ViewManipulate(glm::value_ptr(viewMatrix), 10.0f, { ImGuiCore::GetSceneViewPositionX() + ImGuiCore::GetSceneViewWidth() - renderer.GetWindow()->GetWidth() / 11.25f, ImGuiCore::GetSceneViewPositionY() }, { renderer.GetWindow()->GetWidth() / 11.25f, renderer.GetWindow()->GetWidth() / 11.25f }, 0x0000000);

        // Decompose modified view matrix to get new yaw and pitch
        Matrix4x4 inverted = glm::inverse(viewMatrix);
        Vector3 direction = -Vector3(inverted[2]);

        // Update the local yaw and pitch
        float newYaw = camera.GetYaw();
        float newPitch = camera.GetPitch();

        float deltaYaw = glm::degrees(glm::atan(direction.z, direction.x)) - newYaw;
        newYaw += deltaYaw;

        float deltaPitch = glm::degrees(glm::asin(-direction.y)) - newPitch;
        newPitch += deltaPitch;

        newPitch = Math::Clamp(newPitch, -85.0f, 85.0f);

        // Apply yaw and pitch changes to the camera
        Transform &cameraTransform = camera.GetComponent<Transform>();
        cameraTransform.SetRotation(newYaw, newPitch, NO_CHANGE);

        // If an object is selected
        if (!EngineCore::GetSelectedEntity().IsNull())
        {
            // Scissor gizmos so they don't go beyond the window
            sceneDrawList->PushClipRect({ ImGuiCore::GetSceneViewPositionX(), ImGuiCore::GetSceneViewPositionY() }, { ImGuiCore::GetSceneViewPositionX() + ImGuiCore::GetSceneViewWidth(), ImGuiCore::GetSceneViewPositionY() + ImGuiCore::GetSceneViewHeight() });

            Transform &transform = EngineCore::GetSelectedEntity().GetComponent<Transform>();

            // Convert object's transform into an array
            Matrix4x4 modelMatrix = MatrixUtilities::CreateModel(transform.GetWorldPositionUpInverted(), transform.GetRotation(), transform.GetScale());

            // Set snapping
            float snapDeterminant = Input::GetKeyHeld(GLFW_KEY_LEFT_SHIFT) ? (operation == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f) : 0.0f;
            float snapping[3] = { snapDeterminant, snapDeterminant, snapDeterminant };

            // Show gizmos
            ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), operation, ImGuizmo::MODE::LOCAL, glm::value_ptr(modelMatrix), nullptr, snapping);

            // Retrieve and apply gizmo's changes
            if (ImGuizmo::IsUsing())
            {
                // Decompose model matrix
                Vector3 translation, rotation, scale;
                if (!MatrixUtilities::DecomposeModelMatrix(modelMatrix, translation, rotation, scale)) return;

                // Swap X and Y rotation to suit the engine's axis
                float y = rotation.y;
                rotation.y = rotation.x;
                rotation.x = y;

                // Apply position, rotation, and scale changes whilst inverting Y position, as it has already been inverted once to suit Vulkan's -Y needs
                transform.SetWorldPositionUpInverted(translation);
                transform.SetRotation(rotation);
                transform.SetScale(scale);
            }
        }
    }

    void DebugPanel::DrawUI()
    {
        // Create debug tab
        if (GUI::BeginWindow("Debug Information", nullptr, ImGuiWindowFlags_NoNav))
        {
            ImGui::Text("CPU Frame Time: %i FPS", Time::GetFPS());
            ImGui::Text("GPU Indexed Time: %f ms", renderer.GetTotalDrawTime());
            ImGui::Separator();
            ImGui::Text("Total meshes being drawn: %i", Mesh::GetTotalMeshCount());
            ImGui::Text("Total vertices in scene: %llu", renderer.GetTotalVerticesDrawn());

            GUI::EndWindow();
        }
    }

    void DetailedDebugPanel::DrawUI()
    {
        if (GUI::BeginWindow("Detailed Stats"))
        {
            ImGui::Text("GPU Indexed Time: %fms", renderer.GetTotalDrawTime());

            static constexpr uint SAMPLE_COUNT = 200;
            static constexpr uint REFRESH_RATE = 60;

            static uint currentSampleIndex = 0;

            static float drawTimeSamples[SAMPLE_COUNT] = {};
            static float frameTimeSamples[SAMPLE_COUNT] = {};

            static double refreshTime = Time::GetUpTime();
            while (refreshTime < Time::GetUpTime())
            {
                drawTimeSamples[currentSampleIndex] = renderer.GetTotalDrawTime();
                frameTimeSamples[currentSampleIndex] = Time::GetFPS();

                currentSampleIndex = (currentSampleIndex + 1) % SAMPLE_COUNT;
                refreshTime += 1.0f / REFRESH_RATE;
            }

            float averageDrawTime = 0.0f;
            for (int i = SAMPLE_COUNT; i--;) averageDrawTime += drawTimeSamples[i];
            averageDrawTime /= (float) SAMPLE_COUNT;

            float averageFrameTime = 0.0f;
            for (int i = SAMPLE_COUNT; i--;) averageFrameTime += frameTimeSamples[i];
            averageFrameTime /= (float) SAMPLE_COUNT;

            char drawTimeOverlay[32];
            snprintf(drawTimeOverlay, 32, "Average: %f", averageDrawTime);
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::PlotLines("Lines", drawTimeSamples, SAMPLE_COUNT, currentSampleIndex, drawTimeOverlay, 0.0f, 100.0f, ImVec2(0, 80.0f));

            ImGui::Separator();

            ImGui::Text("CPU Frame Time: %i FPS", Time::GetFPS());

            char frameTimeOverlay[32];
            snprintf(frameTimeOverlay, 32, "Average: %f", averageFrameTime);
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::PlotLines("Lines", frameTimeSamples, SAMPLE_COUNT, currentSampleIndex, frameTimeOverlay, 0.0f, 1000.0f, ImVec2(0, 80.0f));

            GUI::EndWindow();
        }
    }

    void GamePadDebugPanel::DrawUI()
    {
        for (uint i = Input::MAX_GAME_PADS; i--;)
        {
            if (Input::GetGamePadConnected(i))
            {
                bool gamePadInfoOpen = true;
                if (GUI::BeginWindow((fmt::format("Game Pad [{0}] Data", i).c_str()), &gamePadInfoOpen, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("%s", ("Game pad [" + Input::GetGamePadName(i) + "] properties:").c_str());
                    ImGui::Text("%s", fmt::format("Left gamepad stick: [{0}, {1}]", Input::GetGamePadLeftStickAxis(i).x, Input::GetGamePadLeftStickAxis(i).y).c_str());
                    ImGui::Text("%s", fmt::format("Right gamepad stick: [{0}, {1}]", Input::GetGamePadRightStickAxis(i).x, Input::GetGamePadRightStickAxis(i).y).c_str());
                    ImGui::Text("%s", fmt::format("Left trigger: [{0}]", Input::GetGamePadLeftTriggerAxis(i)).c_str());
                    ImGui::Text("%s", fmt::format("Right trigger: [{0}]", Input::GetGamePadRightTriggerAxis(i)).c_str());
                    ImGui::RadioButton("\"A\" pressed", Input::GetGamePadButtonPressed(GLFW_GAMEPAD_BUTTON_A, i));
                    ImGui::RadioButton("\"A\" held", Input::GetGamePadButtonHeld(GLFW_GAMEPAD_BUTTON_A, i));
                    ImGui::RadioButton("\"A\" released", Input::GetGamePadButtonReleased(GLFW_GAMEPAD_BUTTON_A, i));

                    GUI::EndWindow();
                }
            }
        }
    }
}