
#include "GameObject.h"
#include "Transform.h"
#include <iostream>
#include "gcml.h"

#include "../Windows/LoggerWindow.h"

extern LoggerWindow *g_LoggerWindow;

GameObject::GameObject(int id, const std::string &name)
    : id(id), name(name)
{
}

int GameObject::GetComponentCount() const
{
    return static_cast<int>(components.size());
}

std::string GameObject::GetName() const
{
    return name;
}



void GameObject::AddComponent(const std::shared_ptr<Component> &component)
{
    component->SetOwner(this);
    components[component->GetName()] = component;
    // std::cout << "Added " << component->GetName() << std::endl;
}

std::shared_ptr<Component> GameObject::GetComponentByName(const std::string &name) const
{
    auto it = components.find(name);
    if (it != components.end())
    {
        return it->second;
    }
    return nullptr; // Component not found
}

YAML::Node GameObject::Serialize()
{
    YAML::Node node;
    node["ID"] = id;
    node["Name"] = name;

    YAML::Node componentsNode;
    for (const auto &compPair : components)
    {
        const std::string &compName = compPair.first;
        std::shared_ptr<Component> component = compPair.second;
        componentsNode[compName] = component->Serialize();
    }

    node["Components"] = componentsNode;
    return node;
}

void GameObject::Deserialize(const YAML::Node &node)
{

    if (node["ID"])
    {
        id = node["ID"].as<int>();
    }
    if (node["Name"])
    {
        name = node["Name"].as<std::string>();
    }
    if (node["Components"])
    {
        YAML::Node componentsNode = node["Components"];

        for (auto it = componentsNode.begin(); it != componentsNode.end(); ++it)
        {

            std::string compName = it->first.as<std::string>();
            YAML::Node compNode = it->second;

            if (compName == TransformComponent::GetStaticName())
            {
                auto transform = std::make_shared<TransformComponent>();
                transform->Deserialize(compNode);
                AddComponent(transform);
            }
            else if (compName == MeshComponent::GetStaticName())
            {
                auto mesh = std::make_shared<MeshComponent>();
                mesh->Deserialize(compNode);
                AddComponent(mesh);
            }
            else if (compName == ScriptComponent::GetStaticName())
            {
                auto ScriptComp = std::make_shared<ScriptComponent>();
                ScriptComp->Deserialize(compNode);
                AddComponent(ScriptComp);
            }
            else
            {
                g_LoggerWindow->AddLog("[SceneManager] Failed to load Component:  %s", compName.c_str());
                DEBUG_PRINT("[SceneManager] Failed to load Component: %s", compName.c_str());
            }
            // Add deserialization for other components as needed
        }
    }
}

//}
// else if (compName == MeshComponent::GetStaticName())
//{
//    auto render = std::make_shared<MeshComponent>();
//    render->Deserialize(compNode);
//    AddComponent(render);
//}
// else if (compName == MeshComponent::GetStaticName())
//{
//    auto render = std::make_shared<MeshComponent>();
//    render->Deserialize(compNode);
//    AddComponent(render);