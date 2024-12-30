#pragma once

// Component.h

#include <string>
#include <yaml-cpp/yaml.h>

// Forward declaration to avoid circular dependency
class GameObject;

class Component
{
public:
    // Constructor accepting a pointer to the owning GameObject
    Component() {}

    // Virtual destructor
    virtual ~Component() {}

    // Pure virtual methods
    virtual const std::string &GetName() const = 0;

    void SetOwner(GameObject *owner)
    {

        m_Owner = owner;
    }

    virtual void Update(float deltaTime) = 0;

    // Serialization methods
    virtual YAML::Node Serialize() = 0;
    virtual void Deserialize(const YAML::Node &node) = 0;

    // Getter for the owning GameObject
    GameObject *GetOwner() const { return m_Owner; }

protected:
    GameObject *m_Owner; // Pointer to the owning GameObject
};
