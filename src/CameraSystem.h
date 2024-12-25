// src/CameraSystem.h
#pragma once

#include "ECS.h"
#include "CameraComponent.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class CameraSystem {
public:
    CameraSystem(GLFWwindow* window, float deltaTime)
        : window(window), deltaTime(deltaTime) {}

    void ProcessInput(EntityManager& em, ComponentManager& cm) {
        // Iterate through entities with CameraComponent
        const auto& entities = em.GetActiveEntities();
        for(auto entity : entities) {
            if(cm.HasComponent<CameraComponent>(entity)) {
                auto& camera = cm.GetComponent<CameraComponent>(entity);
                // Implement camera movement based on input
                if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                    camera.ProcessKeyboard(0, deltaTime); // Forward
                if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                    camera.ProcessKeyboard(1, deltaTime); // Backward
                if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                    camera.ProcessKeyboard(2, deltaTime); // Left
                if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                    camera.ProcessKeyboard(3, deltaTime); // Right
                if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                    camera.ProcessKeyboard(4, deltaTime); // Up
                if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                    camera.ProcessKeyboard(5, deltaTime); // Down
            }
        }
    }

private:
    GLFWwindow* window;
    float deltaTime;
};
