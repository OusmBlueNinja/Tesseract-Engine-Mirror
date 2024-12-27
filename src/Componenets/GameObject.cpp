
#include "GameObject.h"
#include "Transform.h"
#include <iostream>

GameObject::GameObject(int id, const std::string &name)
    : id(id), name(name)    
{
}

int GameObject::GetComponentCount() const
{
    return static_cast<int>(components.size());
}


void GameObject::AddComponent(const std::shared_ptr<Component> &component)
{
    components[component->GetName()] = component;
    //std::cout << "Added " << component->GetName() << std::endl;
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
            else
            {
                std::cout << "[Poly] [De/Serialize] [ERROR] Invalid Component Type '" << compName << "' Skipping" << std::endl;
            }
            // Add deserialization for other components as needed
        }
    }
}





//}
//else if (compName == MeshComponent::GetStaticName())
//{
//    auto render = std::make_shared<MeshComponent>();
//    render->Deserialize(compNode);
//    AddComponent(render);
//}
//else if (compName == MeshComponent::GetStaticName())
//{
//    auto render = std::make_shared<MeshComponent>();
//    render->Deserialize(compNode);
//    AddComponent(render);