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

// External LoggerWindow instance for logging
extern LoggerWindow *g_LoggerWindow;

// External GameObjects list
extern std::vector<std::unique_ptr<GameObject>> g_GameObjects;

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
    lua_setglobal(m_LuaState, "Engine");

    // Bind the Log function to the Engine table
    lua_getglobal(m_LuaState, "Engine");
    lua_pushcfunction(m_LuaState, Lua_Engine_Log);
    lua_setfield(m_LuaState, -2, "Log");

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

// Binding function to log messages from Lua
int LuaManager::Lua_Engine_Log(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 1 || !lua_isstring(L, 1))
    {
        lua_pushstring(L, "Engine.Log expects at least one string argument.");
        lua_error(L);
        return 0;
    }
    std::string message = lua_tostring(L, 1);
    ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Default white

    // Optional color table
    if (argc >= 2 && lua_istable(L, 2))
    {
        lua_getfield(L, 2, "r");
        if (lua_isnumber(L, -1))
            color.x = lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "g");
        if (lua_isnumber(L, -1))
            color.y = lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "b");
        if (lua_isnumber(L, -1))
            color.z = lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "a");
        if (lua_isnumber(L, -1))
            color.w = lua_tonumber(L, -1);
        lua_pop(L, 1);
    }

    // Log the message
    if (g_LoggerWindow)
    {
        g_LoggerWindow->AddLog(message.c_str(), std::optional<ImVec4>(color));
    }

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

// Binding function to retrieve a MeshComponent's mesh data
int LuaManager::Lua_MeshComponent_GetMeshData(lua_State *L)
{
    // Ensure the first argument is a userdata with MeshComponentMetaTable
    MeshComponent **udata = (MeshComponent **)luaL_checkudata(L, 1, "MeshComponentMetaTable");
    if (udata == nullptr || *udata == nullptr)
    {
        lua_pushstring(L, "Invalid MeshComponent.");
        lua_error(L);
        return 0;
    }

    // Example: Push mesh data as a table
    // Ensure MeshData is defined appropriately in MeshComponent
    GLuint ID = (*udata)->textureID; // Assume MeshData is a struct with relevant fields

    lua_newtable(L);
    lua_pushnumber(L, ID);
    lua_setfield(L, -2, "vertexData");
    // lua_pushstring(L, ID.indexData.c_str());
    // lua_setfield(L, -2, "indexData");
    //  Add more mesh data fields as needed

    return 1; // Return the mesh data table
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

    // Add methods specific to MeshComponent
    lua_pushcfunction(m_LuaState, Lua_MeshComponent_GetMeshData);
    lua_setfield(m_LuaState, -2, "GetMeshData");

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
