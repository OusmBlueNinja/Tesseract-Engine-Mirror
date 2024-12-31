// LuaAPI.h

#pragma once

// Include Lua headers
extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

// Include standard libraries
#include <string>
#include <optional>
#include <variant>
#include <unordered_map>
#include <vector>

// Forward declarations to avoid circular dependencies
class Component;
class TransformComponent;
class MeshComponent;
class ScriptComponent;
class GameObject;
class LoggerWindow;

// LuaManager class definition
class LuaManager
{
public:
    // Constructor
    LuaManager();

    // Destructor
    ~LuaManager();

    /**
     * @brief Initializes the LuaManager with the specified Lua script.
     *
     * This function creates a new Lua state, opens standard libraries,
     * registers all necessary metatables, binds essential functions to Lua,
     * and executes the provided Lua script.
     *
     * @param scriptPath The file path to the Lua script to execute.
     * @return true if initialization is successful; false otherwise.
     */
    bool Initialize(const std::string &scriptPath);

    /**
     * @brief Updates the LuaManager each frame.
     *
     * This function calls the Lua `OnUpdate` function, passing the
     * delta time since the last frame. It handles any errors that
     * occur during the execution of the Lua function.
     *
     * @param deltaTime The time elapsed since the last frame.
     */
    void Update(float deltaTime);

    void CallLuaFunction(std::string functionName);

    using LuaExposedVariant = std::variant<int, float, std::string, bool>;


    
    // Retrieve exposed variables
    std::unordered_map<std::string, LuaExposedVariant> GetExposedVariables();
    void UpdateVariable(const std::string& name, const LuaExposedVariant& value);


private:
    // Lua state
    std::string ScriptPath;

    static std::string m_ScriptName;

    lua_State *m_LuaState;

    static std::unordered_map<std::string, LuaExposedVariant> m_ExposedVariables;


    // Last error message to prevent duplicate logging
    std::string m_LastErrorMessage;
    void RegisterAllMetatables();
    void RegisterComponentMetaTable();

    void RegisterTransformComponentMetaTable();
    void RegisterMeshComponentMetaTable();
    void RegisterScriptComponentMetaTable();
    void RegisterGameObjectMetatable();

    // Binding functions for Component
    static int Lua_Component_GetName(lua_State *L);

    // Binding functions for TransformComponent
    static int Lua_TransformComponent_GetPosition(lua_State *L);
    static int Lua_TransformComponent_SetPosition(lua_State *L);

    static int Lua_TransformComponent_GetRotation(lua_State *L);
    static int Lua_TransformComponent_SetRotation(lua_State *L);

    // Binding functions for MeshComponent
    static int Lua_MeshComponent_GetMeshData(lua_State *L);

    // Binding functions for ScriptComponent
    static int Lua_ScriptComponent_GetScriptPath(lua_State *L);

    // Binding functions for GameObject
    static int Lua_GameObject_GetName(lua_State *L);
    static int Lua_GameObject_GetComponent(lua_State *L);
    static int Lua_GetGameObjectByTag(lua_State *L);

    // Binding functions for Engine table
    static int Lua_Engine_Log(lua_State *L);
    static int Lua_Engine_ScriptName(lua_State *L);
    static int Lua_Engine_GetGameObjectByTag(lua_State *L);
    static int Lua_Engine_Expose(lua_State* L);

    

};
