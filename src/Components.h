// Components.h
#pragma once
#include <string>
#include <glm/glm.hpp> // Ensure you have GLM installed and linked

struct TransformComponent {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    TransformComponent()
        : position(0.0f), rotation(0.0f), scale(1.0f) {}
};

struct SpriteComponent {
    std::string texturePath;
    // Add more sprite-related properties here

    SpriteComponent(const std::string& path = "")
        : texturePath(path) {}
};
