// src/Components/GameObject.h
#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "Component.h"
#include "Transform.h"
#include "Mesh.h"

#include <yaml-cpp/yaml.h>

//     GetComponent<CameraComponent>()

class GameObject
{
public:
    int id;
    std::string name;
    std::unordered_map<std::string, std::shared_ptr<Component>> components;

    int GetComponentCount() const;

    GameObject(int id, const std::string &name);

    void AddComponent(const std::shared_ptr<Component> &component);
    std::shared_ptr<Component> GetComponentByName(const std::string &name) const;

    template <typename T>
    std::shared_ptr<T> GetComponent()
    {
        auto it = components.find(T::GetStaticName());
        if (it != components.end())
        {
            return std::dynamic_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    // Serialization methods
    YAML::Node Serialize();
    void Deserialize(const YAML::Node &node);
};
