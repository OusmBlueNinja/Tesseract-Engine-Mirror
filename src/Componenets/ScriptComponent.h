// ScriptComponent.hpp

#pragma once

#include "Component.h"
#include <string>
#include <yaml-cpp/yaml.h>
#include "Windows/LoggerWindow.h"
#include "Engine/LuaAPI.h" // Include the LuaManager class
#include <unordered_map>

class ScriptComponent : public Component
{
public:
    ScriptComponent();
    virtual ~ScriptComponent();

    std::string ScriptPath; // Path to the Lua script

    // Component interface implementation
    virtual const std::string &GetName() const override;
    static const std::string &GetStaticName();

    virtual YAML::Node Serialize() override;
    virtual void Deserialize(const YAML::Node &node) override;

    // Script management methods
    bool Initialize();
    virtual void Update(float deltaTime);

    void Init();

    void UpdateVariable(const std::string &name, const LuaManager::LuaExposedVariant &value);
    std::unordered_map<std::string, LuaManager::LuaExposedVariant> GetExposedVariables();

private:
    LuaManager m_LuaManager;        // Instance of LuaManager
    std::string m_LastErrorMessage; // To prevent duplicate error logs

    // block copying
    ScriptComponent(const ScriptComponent &) = delete;
    ScriptComponent &operator=(const ScriptComponent &) = delete;

    static const std::string name;
};
