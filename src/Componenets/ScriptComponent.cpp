// ScriptComponent.cpp

#include "ScriptComponent.h"
#include <iostream>

#include "gcml.h"

// External pointer to LoggerWindow (Assuming it's defined globally)
extern LoggerWindow* g_LoggerWindow;



const std::string ScriptComponent::name = "ScriptComponent";


ScriptComponent::ScriptComponent()
    : ScriptPath(""), m_LastErrorMessage("")
{
    Initialize();
}

ScriptComponent::~ScriptComponent()
{
    // LuaManager destructor will handle Lua state cleanup
}

const std::string &ScriptComponent::GetName() const
{
    return name;
}

const std::string &ScriptComponent::GetStaticName()
{
    return name;
}

YAML::Node ScriptComponent::Serialize()
{
    YAML::Node node;

    // ScriptPath
    node["ScriptPath"] = ScriptPath;

    return node;
}

void ScriptComponent::Deserialize(const YAML::Node& node)
{
    if (node["ScriptPath"])
    {
        ScriptPath = node["ScriptPath"].as<std::string>();
    } 
    
    DEBUG_PRINT("Script Path: %s", ScriptPath.c_str());

    Initialize();

}

bool ScriptComponent::Initialize()
{
    if (ScriptPath.empty())
    {
        if (g_LoggerWindow)
        {
            g_LoggerWindow->AddLog("ScriptComponent: Script path is empty.", ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        }
        else
        {
            std::cerr << "ScriptComponent: Script path is empty." << std::endl;
        }
        return false;
    }

    // Initialize LuaManager with the script path
    if (!m_LuaManager.init(ScriptPath))
    {
        if (g_LoggerWindow)
        {
            g_LoggerWindow->AddLog("ScriptComponent: Failed to initialize LuaManager.", ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        }
        else
        {
            std::cerr << "ScriptComponent: Failed to initialize LuaManager." << std::endl;
        }
        return false;
    }

    return true;
}

void ScriptComponent::Update(float deltaTime)
{
    // Call the Update method of LuaManager
    m_LuaManager.onUpdate(deltaTime);
}
