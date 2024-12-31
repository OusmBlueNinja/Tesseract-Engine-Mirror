// LuaAPI.cpp

#include "LuaAPI.h"
#include "LuaMacros.h" // Include the macros for binding
#include "gcml.h"      // Include gcml.h for DEBUG_PRINT macros
#include "Componenets/Component.h"
#include "Componenets/Transform.h"
#include "Componenets/Mesh.h"
#include "Componenets/ScriptComponent.h"
#include "Componenets/GameObject.h"
#include "Windows/LoggerWindow.h"

#include <yaml-cpp/yaml.h>
#include <cstring>
#include <memory>
#include <vector>
#include <filesystem> // C++17 or later

// TODO: Add camera component Meta Table

// External LoggerWindow instance for logging
extern LoggerWindow *g_LoggerWindow;

// External GameObjects list
extern std::vector<std::unique_ptr<GameObject>> g_GameObjects;

std::string LuaManager::m_ScriptName = "LUA_UNDEFINED";
std::unordered_map<std::string, LuaManager::LuaExposedVariant> LuaManager::m_ExposedVariables;


// Constructor
LuaManager::LuaManager()
    : ScriptPath(""), m_LuaState(nullptr), m_LastErrorMessage("")
{
}

// Destructor
LuaManager::~LuaManager()
{
    if (m_LuaState)
    {
        lua_close(m_LuaState);
        m_LuaState = nullptr;
    }
}

// Initialize the LuaManager with the given script path
bool LuaManager::Initialize(const std::string &scriptPath)
{
    if (scriptPath.empty())
    {
        if (g_LoggerWindow)
        {
            g_LoggerWindow->AddLog("LuaManager: Script path is empty.", std::optional<ImVec4>(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)));
        }
        else
        {
            DEBUG_PRINT("LuaManager: Script path is empty.");
        }
        return false;
    }

    ScriptPath = scriptPath;

    m_ScriptName = std::filesystem::path(scriptPath).filename().string();

    // Create a new Lua state
    m_LuaState = luaL_newstate();
    if (!m_LuaState)
    {
        if (g_LoggerWindow)
        {
            g_LoggerWindow->AddLog("LuaManager: Failed to create Lua state.", std::optional<ImVec4>(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)));
        }
        else
        {
            DEBUG_PRINT("LuaManager: Failed to create Lua state.");
        }
        return false;
    }

    // Open Lua standard libraries
    luaL_openlibs(m_LuaState);

    // Register all necessary metatables
    RegisterAllMetatables();

    // Create the Engine table
    lua_newtable(m_LuaState);
    lua_setglobal(m_LuaState, "_T_Engine_Table");

    // Bind the Log function to the Engine table
    lua_getglobal(m_LuaState, "_T_Engine_Table");
    lua_pushcfunction(m_LuaState, Lua_Engine_Log);
    lua_setfield(m_LuaState, -2, "Log");

    lua_pushcfunction(m_LuaState, Lua_Engine_Expose);
    lua_setfield(m_LuaState, -2, "Expose");

    // Add the ScriptName binding
    lua_pushcfunction(m_LuaState, Lua_Engine_ScriptName);
    lua_setfield(m_LuaState, -2, "ScriptName");

    // Bind the GetGameObjectByTag function to the Engine table
    lua_pushcfunction(m_LuaState, Lua_Engine_GetGameObjectByTag);
    lua_setfield(m_LuaState, -2, "GetGameObjectByTag");

    lua_pop(m_LuaState, 1); // Pop the Engine table from the stack

    // Execute the Lua script
    if (luaL_dofile(m_LuaState, ScriptPath.c_str()) != LUA_OK)
    {
        const char *luaError = lua_tostring(m_LuaState, -1);
        if (luaError)
        {
            std::string errorMsg(luaError);
            // Prevent duplicate error logs
            if (errorMsg != m_LastErrorMessage)
            {
                if (g_LoggerWindow)
                {
                    std::string formattedError = "LuaManager Error: " + errorMsg;
                    ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                    g_LoggerWindow->AddLog(formattedError.c_str(), std::optional<ImVec4>(redColor));
                }
                else
                {
                    DEBUG_PRINT("LuaManager Error: %s", errorMsg.c_str());
                }
                m_LastErrorMessage = errorMsg;
            }
        }
        else
        {
            if (g_LoggerWindow)
            {
                g_LoggerWindow->AddLog("LuaManager: Unknown error executing script.", std::optional<ImVec4>(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)));
            }
            else
            {
                DEBUG_PRINT("LuaManager: Unknown error executing script.");
            }
        }

        lua_pop(m_LuaState, 1); // Remove error message from stack
        return false;
    }

    // Reset last error message on successful script execution
    m_LastErrorMessage.clear();

    // Log successful initialization
    DEBUG_PRINT("LuaManager initialized successfully with script: %s", ScriptPath.c_str());

    return true;
}

std::unordered_map<std::string, LuaManager::LuaExposedVariant> LuaManager::GetExposedVariables() {
    return m_ExposedVariables;
}



void LuaManager::UpdateVariable(const std::string &name, const LuaManager::LuaExposedVariant &value) {
    if (!m_LuaState)
        return;

    // Update the variable in the map
    m_ExposedVariables[name] = value;

    // Push the variable to the Lua global environment
    lua_pushglobaltable(m_LuaState); // Push the global table

    // Push the variable name
    lua_pushstring(m_LuaState, name.c_str());

    // Push the new value based on its type
    if (std::holds_alternative<int>(value)) {
        lua_pushinteger(m_LuaState, std::get<int>(value));
    } else if (std::holds_alternative<float>(value)) {
        lua_pushnumber(m_LuaState, std::get<float>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        lua_pushstring(m_LuaState, std::get<std::string>(value).c_str());
    } else if (std::holds_alternative<bool>(value)) {
        lua_pushboolean(m_LuaState, std::get<bool>(value));
    } else {
        lua_pop(m_LuaState, 1); // Clean up stack
        return;
    }

    // Set the variable in the Lua global environment
    lua_settable(m_LuaState, -3);

    // Clean up stack
    lua_pop(m_LuaState, 1);
}




int LuaManager::Lua_Engine_Expose(lua_State* L) {
    // Check that at least two arguments are passed: name and value
    if (lua_gettop(L) < 2) {
        luaL_error(L, "Expose function requires at least two arguments: name and value");
        return 0;
    }

    // First argument: variable name (string)
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "First argument to Expose must be a string (variable name)");
        return 0;
    }

    const char* varName = lua_tostring(L, 1);

    // Second argument: variable value (supports int, float, string, bool)
    LuaExposedVariant varValue;
    int type = lua_type(L, 2);

    switch (type) {
        case LUA_TNUMBER:
#if LUA_VERSION_NUM >= 503
            if (lua_isinteger(L, 2)) {
                varValue = static_cast<int>(lua_tointeger(L, 2));
            } else
#endif
            {
                varValue = static_cast<float>(lua_tonumber(L, 2));
            }
            break;
        case LUA_TSTRING:
            varValue = std::string(lua_tostring(L, 2));
            break;
        case LUA_TBOOLEAN:
            varValue = static_cast<bool>(lua_toboolean(L, 2));
            break;
        default:
            luaL_error(L, "Unsupported variable type for Expose");
            return 0;
    }

    // Store the variable in the static m_ExposedVariables map
    m_ExposedVariables[varName] = varValue;

    // Push the variable to the Lua global environment
    lua_pushglobaltable(L);

    lua_pushstring(L, varName); // Push the variable name

    // Push the value to Lua based on its type
    if (std::holds_alternative<int>(varValue)) {
        lua_pushinteger(L, std::get<int>(varValue));
    } else if (std::holds_alternative<float>(varValue)) {
        lua_pushnumber(L, std::get<float>(varValue));
    } else if (std::holds_alternative<std::string>(varValue)) {
        lua_pushstring(L, std::get<std::string>(varValue).c_str());
    } else if (std::holds_alternative<bool>(varValue)) {
        lua_pushboolean(L, std::get<bool>(varValue));
    } else {
        lua_pop(L, 1); // Clean up stack
        return 0;
    }

    // Set the variable in Lua global environment
    lua_settable(L, -3);

    // Clean up stack
    lua_pop(L, 1);

    return 0;
}



// Update function called every frame
void LuaManager::Update(float deltaTime)
{

    if (!m_LuaState)
    {
        if (g_LoggerWindow)
        {
            g_LoggerWindow->AddLog("LuaManager: Lua state is not initialized.", std::optional<ImVec4>(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)));
        }
        else
        {
            DEBUG_PRINT("LuaManager: Lua state is not initialized.");
        }
        return;
    }

    // Push the 'OnUpdate' function onto the stack
    lua_getglobal(m_LuaState, "OnUpdate");
    if (!lua_isfunction(m_LuaState, -1))
    {
        if (g_LoggerWindow)
        {
            g_LoggerWindow->AddLog("LuaManager: 'OnUpdate' is not a function.", std::optional<ImVec4>(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)));
        }
        else
        {
            DEBUG_PRINT("LuaManager: 'OnUpdate' is not a function.");
        }
        lua_pop(m_LuaState, 1); // Remove non-function value from stack
        return;
    }

    // Push the deltaTime argument
    lua_pushnumber(m_LuaState, deltaTime);

    // Call the 'OnUpdate' function with 1 argument and 0 return values
    if (lua_pcall(m_LuaState, 1, 0, 0) != LUA_OK)
    {
        const char *luaError = lua_tostring(m_LuaState, -1);
        if (luaError)
        {
            std::string errorMsg(luaError);
            // Prevent duplicate error logs
            if (errorMsg != m_LastErrorMessage)
            {
                if (g_LoggerWindow)
                {
                    std::string formattedError = "LuaManager Error in OnUpdate: " + errorMsg;
                    ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                    g_LoggerWindow->AddLog(formattedError.c_str(), std::optional<ImVec4>(redColor));
                }
                else
                {
                    DEBUG_PRINT("LuaManager Error in OnUpdate: %s", errorMsg.c_str());
                }
                m_LastErrorMessage = errorMsg;
            }
        }
        else
        {
            if (g_LoggerWindow)
            {
                g_LoggerWindow->AddLog("LuaManager: Unknown error in OnUpdate.", std::optional<ImVec4>(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)));
            }
            else
            {
                DEBUG_PRINT("LuaManager: Unknown error in OnUpdate.");
            }
        }

        lua_pop(m_LuaState, 1); // Remove error message from stack
        return;
    }
    else
    {
        // Reset last error message on successful call
        m_LastErrorMessage.clear();
    }
}

// Update function called every frame
void LuaManager::CallLuaFunction(std::string functionName)
{

    if (!m_LuaState)
    {
        if (g_LoggerWindow)
        {
            g_LoggerWindow->AddLog("LuaManager: Lua state is not initialized.", std::optional<ImVec4>(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)));
        }
        else
        {
            DEBUG_PRINT("LuaManager: Lua state is not initialized.");
        }
        return;
    }

    // Push the 'OnUpdate' function onto the stack
    lua_getglobal(m_LuaState, functionName.c_str());
    if (!lua_isfunction(m_LuaState, -1))
    {
        DEBUG_PRINT("LuaManager: 'OnUpdate' is not a function.");
        return;
    }

    // Call the function with 0 argument and 0 return values
    if (lua_pcall(m_LuaState, 0, 0, 0) != LUA_OK)
    {
        const char *luaError = lua_tostring(m_LuaState, -1);
        if (luaError)
        {
            std::string errorMsg(luaError);
            // Prevent duplicate error logs
            if (errorMsg != m_LastErrorMessage)
            {
                if (g_LoggerWindow)
                {
                    std::string formattedError = "LuaManager Error in " + functionName + ": " + errorMsg;
                    ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                    g_LoggerWindow->AddLog(formattedError.c_str(), std::optional<ImVec4>(redColor));
                }
                else
                {
                    DEBUG_PRINT("LuaManager Error in OnUpdate: %s", errorMsg.c_str());
                }
                m_LastErrorMessage = errorMsg;
            }
        }
        else
        {
            if (g_LoggerWindow)
            {
                g_LoggerWindow->AddLog("LuaManager: Unknown error in %s.", std::optional<ImVec4>(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), functionName.c_str());
            }
            else
            {
                DEBUG_PRINT("LuaManager: Unknown error in %s.", functionName.c_str());
            }
        }

        lua_pop(m_LuaState, 1); // Remove error message from stack
        return;
    }
    else
    {
        // Reset last error message on successful call
        m_LastErrorMessage.clear();
    }
}

int LuaManager::Lua_Engine_ScriptName(lua_State *L)
{

    // Push the script name onto the Lua stack
    lua_pushstring(L, m_ScriptName.c_str());

    // Return 1 value (the string)
    return 1;
}

// Binding function to log messages from Lua
int LuaManager::Lua_Engine_Log(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 1 || !lua_isstring(L, 1))
    {
        luaL_error(L, "Engine.Log expects at least one string argument.");
        return 0; // This line won't be reached due to lua_error
    }

    // Retrieve the log message
    const char *message = lua_tostring(L, 1);

    // Prepend the script name
    std::string formattedMessage = "[" + m_ScriptName + "]: " + message;

    // Default color: white
    ImVec4 color(1.0f, 1.0f, 1.0f, 1.0f);

    // Check if a color table is provided
    if (argc >= 2 && lua_istable(L, 2))
    {
        // Efficiently retrieve color components using numeric indices
        // Assumes color table is an array: {r, g, b, a}
        // lua_geti is faster than lua_getfield for numeric indices

        // r (index 1)
        lua_geti(L, 2, 1);
        if (lua_isnumber(L, -1))
            color.x = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1); // Remove the value from the stack

        // g (index 2)
        lua_geti(L, 2, 2);
        if (lua_isnumber(L, -1))
            color.y = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        // b (index 3)
        lua_geti(L, 2, 3);
        if (lua_isnumber(L, -1))
            color.z = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        // a (index 4)
        lua_geti(L, 2, 4);
        if (lua_isnumber(L, -1))
            color.w = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);
    }

    // Log the message with the specified color

    g_LoggerWindow->AddLog(formattedMessage.c_str(), std::optional<ImVec4>(color));

    return 0; // No return values
}

// Binding function to retrieve a GameObject by tag from Lua
int LuaManager::Lua_Engine_GetGameObjectByTag(lua_State *L)
{
    // Reuse the existing Lua_GetGameObjectByTag function
    return Lua_GetGameObjectByTag(L);
}

// Binding function to retrieve a GameObject by tag
int LuaManager::Lua_GetGameObjectByTag(lua_State *L)
{
    // Check if the first argument is a string
    if (!lua_isstring(L, 1))
    {
        lua_pushstring(L, "GetGameObjectByTag expects a string argument.");
        lua_error(L); // Raises a Lua error and does not return
        return 0;     // This line won't be reached
    }

    std::string tag = lua_tostring(L, 1);

    // Search for the GameObject with the matching tag
    GameObject *foundObject = nullptr;
    for (auto &obj : g_GameObjects)
    {
        if (obj->name == tag)
        {
            foundObject = obj.get();
            break;
        }
    }

    if (foundObject == nullptr)
    {
        lua_pushnil(L); // Push nil to the stack if not found
        return 1;       // Return 1 (nil on Lua stack)
    }

    // Create userdata to hold the GameObject pointer
    GameObject **udata = (GameObject **)lua_newuserdata(L, sizeof(GameObject *));
    *udata = foundObject;

    // Set the metatable
    luaL_getmetatable(L, "GameObjectMetaTable");
    if (!lua_istable(L, -1)) // Check if the metatable was successfully found
    {
        DEBUG_PRINT("LuaManager: Metatable 'GameObjectMetaTable' not found.");
        lua_pop(L, 1);  // Remove the invalid metatable from the stack
        lua_pushnil(L); // Return nil to indicate failure
        return 1;       // Return 1 (nil on Lua stack)
    }

    lua_setmetatable(L, -2); // Set the metatable for the userdata

    return 1; // Return the GameObject userdata
}

// Binding function to retrieve a Component by name from a GameObject
int LuaManager::Lua_GameObject_GetComponent(lua_State *L)
{
    // Ensure the first argument is a userdata with the correct metatable
    GameObject **udata = (GameObject **)luaL_checkudata(L, 1, "GameObjectMetaTable");
    if (udata == nullptr || *udata == nullptr)
    {
        lua_pushstring(L, "Invalid GameObject.");
        lua_error(L);
        return 0;
    }

    // Ensure the second argument is a string representing the component name
    if (!lua_isstring(L, 2))
    {
        lua_pushstring(L, "GetComponent expects a string as the second argument.");
        lua_error(L);
        return 0;
    }

    const char *componentNameStr = lua_tostring(L, 2);

    // Retrieve the component by name
    Component *component = (*udata)->GetComponentByName(componentNameStr).get();

    if (component == nullptr)
    {
        lua_pushnil(L); // Return nil if component not found
        return 1;
    }

    // Determine which metatable to use based on the component type
    if (strcmp(componentNameStr, "Transform") == 0)
    {
        luaL_getmetatable(L, "TransformMetaTable");
    }
    else if (strcmp(componentNameStr, "Mesh") == 0)
    {
        luaL_getmetatable(L, "MeshMetaTable");
    }
    else if (strcmp(componentNameStr, "Script") == 0)
    {
        luaL_getmetatable(L, "ScriptMetaTable");
    }
    else
    {
        lua_pushstring(L, "Unknown ComponentType specified.");
        lua_error(L);
        return 0;
    }

    // Check if the metatable was successfully retrieved
    std::string metatableName = std::string(componentNameStr) + "MetaTable";

    if (!lua_istable(L, -1))
    {
        DEBUG_PRINT("LuaManager: Metatable '%s' not found.", std::string(metatableName).c_str());
        lua_pop(L, 1);  // Remove the invalid metatable from the stack
        lua_pushnil(L); // Return nil to indicate failure
        return 1;
    }

    // Create userdata for the Component
    Component **compUdata = (Component **)lua_newuserdata(L, sizeof(Component *));
    *compUdata = component;

    // Retrieve and push the metatable onto the stack
    luaL_getmetatable(L, std::string(metatableName).c_str());
    if (lua_isnil(L, -1))
    {
        luaL_error(L, "Metatable '%s' not found", std::string(metatableName).c_str());
    }

    // Set the metatable for the userdata
    lua_setmetatable(L, -2);

    return 1; // Return the Component userdata
}

// Binding function to retrieve a Component's name
int LuaManager::Lua_Component_GetName(lua_State *L)
{
    // Ensure the first argument is a userdata with ComponentMetaTable
    Component **udata = (Component **)luaL_checkudata(L, 1, "ComponentMetaTable");
    if (udata == nullptr || *udata == nullptr)
    {
        lua_pushstring(L, "Invalid Component.");
        lua_error(L);
        return 0;
    }

    // Push the name of the Component
    lua_pushstring(L, (*udata)->GetName().c_str());

    return 1; // Return the name
}

// Binding function to retrieve a TransformComponent's position
int LuaManager::Lua_TransformComponent_GetPosition(lua_State *L)
{
    // Ensure the first argument is a userdata with TransformMetaTable
    TransformComponent **udata = (TransformComponent **)luaL_checkudata(L, 1, "TransformMetaTable");
    if (udata == nullptr || *udata == nullptr)
    {
        lua_pushstring(L, "Invalid TransformComponent.");
        lua_error(L);
        return 0;
    }

    // Assuming TransformComponent has a GetPosition method returning a glm::vec3
    glm::vec3 position = (*udata)->GetPosition(); // Example using glm::vec3

    // Push position as a Lua table
    lua_newtable(L);
    lua_pushnumber(L, position.x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, position.y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, position.z);
    lua_setfield(L, -2, "z");

    return 1; // Return the position table
}

// Binding function to set a TransformComponent's position
int LuaManager::Lua_TransformComponent_SetPosition(lua_State *L)
{
    // Ensure the first argument is a userdata with TransformMetaTable
    TransformComponent **udata = (TransformComponent **)luaL_checkudata(L, 1, "TransformMetaTable");
    if (udata == nullptr || *udata == nullptr)
    {
        lua_pushstring(L, "Invalid TransformComponent.");
        lua_error(L);
        return 0;
    }

    // Ensure the second argument is a table with x, y, z
    if (!lua_istable(L, 2))
    {
        lua_pushstring(L, "SetPosition expects a table with x, y, z fields.");
        lua_error(L);
        return 0;
    }

    lua_getfield(L, 2, "x");
    lua_getfield(L, 2, "y");
    lua_getfield(L, 2, "z");

    if (!lua_isnumber(L, -3) || !lua_isnumber(L, -2) || !lua_isnumber(L, -1))
    {
        lua_pushstring(L, "SetPosition expects numerical x, y, z fields.");
        lua_error(L);
        return 0;
    }

    float x = lua_tonumber(L, -3);
    float y = lua_tonumber(L, -2);
    float z = lua_tonumber(L, -1);

    lua_pop(L, 3); // Remove x, y, z from stack

    (*udata)->SetPosition(x, y, z); // Corrected to match the method signature

    return 0; // No return values
}

// Binding function to retrieve a TransformComponent's position
int LuaManager::Lua_TransformComponent_GetRotation(lua_State *L)
{
    // Ensure the first argument is a userdata with TransformMetaTable
    TransformComponent **udata = (TransformComponent **)luaL_checkudata(L, 1, "TransformMetaTable");
    if (udata == nullptr || *udata == nullptr)
    {
        lua_pushstring(L, "Invalid TransformComponent.");
        lua_error(L);
        return 0;
    }

    // Assuming TransformComponent has a GetPosition method returning a glm::vec3
    glm::vec3 rotation = (*udata)->GetRotation(); // Example using glm::vec3

    // Push position as a Lua table
    lua_newtable(L);
    lua_pushnumber(L, rotation.x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, rotation.y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, rotation.z);
    lua_setfield(L, -2, "z");

    return 1; // Return the position table
}

// Binding function to set a TransformComponent's position
int LuaManager::Lua_TransformComponent_SetRotation(lua_State *L)
{
    // Ensure the first argument is a userdata with TransformMetaTable
    TransformComponent **udata = (TransformComponent **)luaL_checkudata(L, 1, "TransformMetaTable");
    if (udata == nullptr || *udata == nullptr)
    {
        lua_pushstring(L, "Invalid TransformComponent.");
        lua_error(L);
        return 0;
    }

    // Ensure the second argument is a table with x, y, z
    if (!lua_istable(L, 2))
    {
        lua_pushstring(L, "SetRotation expects a table with x, y, z fields.");
        lua_error(L);
        return 0;
    }

    lua_getfield(L, 2, "x");
    lua_getfield(L, 2, "y");
    lua_getfield(L, 2, "z");

    if (!lua_isnumber(L, -3) || !lua_isnumber(L, -2) || !lua_isnumber(L, -1))
    {
        lua_pushstring(L, "SetRotation expects numerical x, y, z fields.");
        lua_error(L);
        return 0;
    }

    float x = lua_tonumber(L, -3);
    float y = lua_tonumber(L, -2);
    float z = lua_tonumber(L, -1);

    lua_pop(L, 3); // Remove x, y, z from stack

    (*udata)->SetRotation(x, y, z); // Corrected to match the method signature

    return 0; // No return values
}

// Binding function to retrieve a ScriptComponent's script path
int LuaManager::Lua_ScriptComponent_GetScriptPath(lua_State *L)
{
    // Ensure the first argument is a userdata with ScriptComponentMetaTable
    ScriptComponent **udata = (ScriptComponent **)luaL_checkudata(L, 1, "ScriptComponentMetaTable");
    if (udata == nullptr || *udata == nullptr)
    {
        lua_pushstring(L, "Invalid ScriptComponent.");
        lua_error(L);
        return 0;
    }

    // Push the script path
    lua_pushstring(L, (*udata)->ScriptPath.c_str());

    return 1; // Return the script path
}

// Binding function to retrieve a GameObject's name
int LuaManager::Lua_GameObject_GetName(lua_State *L)
{
    // Ensure the first argument is a userdata with GameObjectMetaTable
    GameObject **udata = (GameObject **)luaL_checkudata(L, 1, "GameObjectMetaTable");
    if (udata == nullptr || *udata == nullptr)
    {
        lua_pushstring(L, "Invalid GameObject.");
        lua_error(L);
        return 0;
    }

    // Push the name of the GameObject
    lua_pushstring(L, (*udata)->GetName().c_str());

    return 1; // Return the name
}

// Function to register all metatables
void LuaManager::RegisterAllMetatables()
{
    RegisterComponentMetaTable();
    RegisterTransformComponentMetaTable();
    RegisterMeshComponentMetaTable();
    RegisterScriptComponentMetaTable();
    RegisterGameObjectMetatable();
}

// Function to register the base ComponentMetaTable
void LuaManager::RegisterComponentMetaTable()
{
    luaL_newmetatable(m_LuaState, "ComponentMetaTable");

    // __index
    lua_pushstring(m_LuaState, "__index");
    lua_newtable(m_LuaState);

    // Add methods to the metatable
    lua_pushcfunction(m_LuaState, Lua_Component_GetName);
    lua_setfield(m_LuaState, -2, "GetName");

    // Add more common methods as needed

    lua_settable(m_LuaState, -3); // Set __index to the table with methods

    lua_pop(m_LuaState, 1); // Pop the metatable
}

// Function to register the TransformMetaTable
void LuaManager::RegisterTransformComponentMetaTable()
{
    luaL_newmetatable(m_LuaState, "TransformMetaTable");

    // __index
    lua_pushstring(m_LuaState, "__index");
    lua_newtable(m_LuaState);

    // Inherit from ComponentMetaTable
    luaL_getmetatable(m_LuaState, "ComponentMetaTable");
    lua_setfield(m_LuaState, -2, "__base");

    // Add methods specific to TransformComponent
    lua_pushcfunction(m_LuaState, Lua_TransformComponent_GetPosition);
    lua_setfield(m_LuaState, -2, "GetPosition");

    lua_pushcfunction(m_LuaState, Lua_TransformComponent_SetPosition);
    lua_setfield(m_LuaState, -2, "SetPosition");

    // Add methods specific to TransformComponent
    lua_pushcfunction(m_LuaState, Lua_TransformComponent_GetRotation);
    lua_setfield(m_LuaState, -2, "GetRotation");

    lua_pushcfunction(m_LuaState, Lua_TransformComponent_SetRotation);
    lua_setfield(m_LuaState, -2, "SetRotation");

    // Add more Transform-specific methods as needed

    lua_settable(m_LuaState, -3); // Set __index to the table with methods

    lua_pop(m_LuaState, 1); // Pop the metatable
}

// Function to register the MeshComponentMetaTable
void LuaManager::RegisterMeshComponentMetaTable()
{
    luaL_newmetatable(m_LuaState, "MeshMetaTable");

    // __index
    lua_pushstring(m_LuaState, "__index");
    lua_newtable(m_LuaState);

    // Inherit from ComponentMetaTable
    luaL_getmetatable(m_LuaState, "ComponentMetaTable");
    lua_setfield(m_LuaState, -2, "__base");

    // Add more Mesh-specific methods as needed

    lua_settable(m_LuaState, -3); // Set __index to the table with methods

    lua_pop(m_LuaState, 1); // Pop the metatable
}

// Function to register the ScriptComponentMetaTable
void LuaManager::RegisterScriptComponentMetaTable()
{
    luaL_newmetatable(m_LuaState, "ScriptMetaTable");

    // __index
    lua_pushstring(m_LuaState, "__index");
    lua_newtable(m_LuaState);

    // Inherit from ComponentMetaTable
    luaL_getmetatable(m_LuaState, "ComponentMetaTable");
    lua_setfield(m_LuaState, -2, "__base");

    // Add methods specific to ScriptComponent
    lua_pushcfunction(m_LuaState, Lua_ScriptComponent_GetScriptPath);
    lua_setfield(m_LuaState, -2, "GetScriptPath");

    // Add more Script-specific methods as needed

    lua_settable(m_LuaState, -3); // Set __index to the table with methods

    lua_pop(m_LuaState, 1); // Pop the metatable
}

// Function to register the GameObjectMetaTable
void LuaManager::RegisterGameObjectMetatable()
{
    luaL_newmetatable(m_LuaState, "GameObjectMetaTable");

    // __index
    lua_pushstring(m_LuaState, "__index");
    lua_newtable(m_LuaState);

    // Add methods to the metatable
    lua_pushcfunction(m_LuaState, Lua_GameObject_GetName);
    lua_setfield(m_LuaState, -2, "GetName");

    lua_pushcfunction(m_LuaState, Lua_GameObject_GetComponent);
    lua_setfield(m_LuaState, -2, "GetComponent");

    // Add more methods as needed

    lua_settable(m_LuaState, -3); // Set __index to the table with methods

    lua_pop(m_LuaState, 1); // Pop the metatable
}
