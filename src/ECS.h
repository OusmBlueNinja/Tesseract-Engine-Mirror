// src/ECS.h
#pragma once

#include <unordered_map>
#include <vector>
#include <typeindex>
#include <typeinfo>
#include <memory>
#include <algorithm>
#include <string>

// Forward declaration
class ComponentManager;

// Type alias for Entity
using Entity = uint32_t;
const Entity MAX_ENTITIES = 5000;

// Simple Vector3 struct to replace glm::vec3
struct Vector3 {
    float x, y, z;

    Vector3(float x_=0.0f, float y_=0.0f, float z_=0.0f)
        : x(x_), y(y_), z(z_) {}
};

// =====================
// Components
// =====================

struct TransformComponent {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;

    TransformComponent()
        : position(0.0f, 0.0f, 0.0f),
          rotation(0.0f, 0.0f, 0.0f),
          scale(1.0f, 1.0f, 1.0f) {}
};

struct SpriteComponent {
    std::string texturePath;
    // Add more sprite-related properties here

    SpriteComponent(const std::string& path = "")
        : texturePath(path) {}
};

// =====================
// ECS Core Classes
// =====================

// Base class for Component Arrays
class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
};

// Template class for storing components of type T
template<typename T>
class ComponentArray : public IComponentArray {
public:
    void InsertData(Entity entity, T component) {
        if (entityToIndex.find(entity) != entityToIndex.end())
            return; // Component already exists for this entity
        size_t newIndex = components.size();
        entityToIndex[entity] = newIndex;
        indexToEntity[newIndex] = entity;
        components.emplace_back(component);
    }

    void RemoveData(Entity entity) {
        auto it = entityToIndex.find(entity);
        if (it == entityToIndex.end())
            return; // Component does not exist

        size_t index = it->second;
        size_t lastIndex = components.size() - 1;
        components[index] = components[lastIndex];

        Entity lastEntity = indexToEntity[lastIndex];
        entityToIndex[lastEntity] = index;
        indexToEntity[index] = lastEntity;

        components.pop_back();
        entityToIndex.erase(entity);
        indexToEntity.erase(lastIndex);
    }

    bool HasData(Entity entity) const {
        return entityToIndex.find(entity) != entityToIndex.end();
    }

    T& GetData(Entity entity) {
        return components.at(entityToIndex.at(entity));
    }

    void EntityDestroyed(Entity entity) override {
        if (entityToIndex.find(entity) != entityToIndex.end()) {
            RemoveData(entity);
        }
    }

private:
    std::vector<T> components;
    std::unordered_map<Entity, size_t> entityToIndex;
    std::unordered_map<size_t, Entity> indexToEntity;
};

// ComponentManager handles all components
class ComponentManager {
public:
    template<typename T>
    void RegisterComponent() {
        const std::type_index typeName = typeid(T);
        if (componentArrays.find(typeName) != componentArrays.end()) {
            // Component already registered
            // Logger is not accessible here; handle logging in main.cpp
            return;
        }
        componentArrays[typeName] = std::make_shared<ComponentArray<T>>();
    }

    template<typename T>
    void AddComponent(Entity entity, T component) {
        GetComponentArray<T>()->InsertData(entity, component);
    }

    template<typename T>
    void RemoveComponent(Entity entity) {
        GetComponentArray<T>()->RemoveData(entity);
    }

    template<typename T>
    bool HasComponent(Entity entity) const {
        return GetComponentArray<T>()->HasData(entity);
    }

    template<typename T>
    T& GetComponent(Entity entity) {
        return GetComponentArray<T>()->GetData(entity);
    }

    void EntityDestroyed(Entity entity) {
        for (auto const& pair : componentArrays) {
            auto const& component = pair.second;
            component->EntityDestroyed(entity);
        }
    }

private:
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> componentArrays;

    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() const {
        const std::type_index typeName = typeid(T);
        auto it = componentArrays.find(typeName);
        if (it == componentArrays.end()) {
            throw std::runtime_error("Component not registered.");
        }
        return std::static_pointer_cast<ComponentArray<T>>(it->second);
    }
};

// EntityManager handles entity creation and destruction
class EntityManager {
public:
    EntityManager() : livingEntityCount(0) {}

    void Init() {
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
            availableEntities.push_back(entity);
    }

    Entity CreateEntity() {
        if (livingEntityCount >= MAX_ENTITIES) {
            throw std::runtime_error("Too many entities in existence.");
        }
        Entity id = availableEntities.back();
        availableEntities.pop_back();
        activeEntities.emplace_back(id);
        ++livingEntityCount;
        return id;
    }

    void DestroyEntity(Entity entity, ComponentManager& cm) {
        if (entity >= MAX_ENTITIES) {
            return;
        }
        availableEntities.push_back(entity);
        activeEntities.erase(std::remove(activeEntities.begin(), activeEntities.end(), entity), activeEntities.end());
        cm.EntityDestroyed(entity);
        --livingEntityCount;
    }

    const std::vector<Entity>& GetActiveEntities() const { return activeEntities; }

private:
    std::vector<Entity> availableEntities{};
    std::vector<Entity> activeEntities{};
    uint32_t livingEntityCount;
};
