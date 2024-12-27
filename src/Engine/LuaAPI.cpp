// LuaManager.cpp

#include "LuaAPI.h"
#include "LuaMacros.h"
#include "imgui.h"
#include <iostream>

#include "gcml.h"

#include "Windows/LoggerWindow.h"

extern LoggerWindow *g_LoggerWindow;

int lua_log_message(lua_State *L);

LuaManager::LuaManager() : L(nullptr), m_firstCall(false), m_Initialized(false) {}

LuaManager::~LuaManager()
{
    if (L)
    {
        lua_close(L);
    }
}

bool LuaManager::init(const std::string &scriptPath)
{
    // Create a new Lua state
    L = luaL_newstate();
    if (L == nullptr)
    {
        std::cerr << "Failed to create Lua state.\n";
        return false;
    }

    // Open Lua standard libraries
    luaL_openlibs(L);

    // Create the Engine table
    CREATE_LUA_TABLE(L, Engine);

    // Bind the Log function to the Engine table
    BIND_LUA_FUNCTION(L, Engine, "Log", lua_log_message);

    // Load and execute the Lua script
    if (luaL_dofile(L, scriptPath.c_str()) != LUA_OK)
    {
        g_LoggerWindow->AddLog("%s", ImVec4(1.0f, 0.0f, 0.0f, 1.0f), lua_tostring(L, -1));
        DEBUG_PRINT("%s: %s", scriptPath.c_str(), lua_tostring(L, -1));
        lua_close(L);
        L = nullptr;

        return false;
    }
    m_Initialized = true;
    callLuaFunction("OnInit");

    return true;
}

bool LuaManager::callLuaFunction(const std::string &funcName)
{
    if (!m_Initialized) {
        return false;
    }
    lua_getglobal(L, funcName.c_str());
    if (!lua_isfunction(L, -1))
    {
        std::cerr << "'" << funcName << "' is not a function.\n";
        lua_pop(L, 1); // Remove non-function value
        return false;
    }

    // Call the function with 0 arguments and 0 return values
    if (lua_pcall(L, 0, 0, 0) != LUA_OK)
    {
        std::cerr << "Error calling '" << funcName << "': " << lua_tostring(L, -1) << "\n";
        lua_pop(L, 1); // Remove error message
        return false;
    }

    return true;
}

bool LuaManager::onUpdate(float deltaTime)
{
    if (!m_Initialized) {
        return false;
    }
    // Push the 'OnUpdate' function onto the stack
    lua_getglobal(L, "OnUpdate"); // Ensure correct case
    if (!lua_isfunction(L, -1))
    {
        std::cerr << "'OnUpdate' is not a function.\n";
        lua_pop(L, 1); // Remove non-function value
        return false;
    }

    // Push the deltaTime argument
    lua_pushnumber(L, deltaTime);

    m_firstCall = true;

    // Call the function with 1 argument and 0 return values
    if (lua_pcall(L, 1, 0, 0) != LUA_OK)
    {
        // Retrieve the error message from Lua
        const char *luaError = lua_tostring(L, -1);
        if (luaError)
        {
            std::string errorMsg(luaError);

            // Check if this error message has already been logged
            if (errorMsg != m_LastErrorMessage)
            {
                // Log the error to the in-game terminal in red
                if (g_LoggerWindow)
                {
                    g_LoggerWindow->AddLog(errorMsg.c_str(), ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                }
                else
                {
                    std::cerr << "LoggerWindow is not initialized.\n";
                }

                // Update the last error message
                m_LastErrorMessage = errorMsg;
            }

            // Optionally, print to std::cerr once if LoggerWindow is unavailable
            if (!g_LoggerWindow)
            {
                std::cerr << "Error calling 'OnUpdate': " << luaError << "\n";
            }
        }
        else
        {
            std::cerr << "Unknown error calling 'OnUpdate'.\n";
        }

        lua_pop(L, 1); // Remove error message

        return false;
    }
    else
    {
        // Reset the last error message if the call was successful
        m_LastErrorMessage.clear();
    }
    return true;
}

bool LuaManager::onDrawGui()
{
    return callLuaFunction("onDrawGui");
}

bool LuaManager::callFunction(const std::string &funcName, int args, int returns)
{
    if (!m_Initialized) {
        return false;
    }
    lua_getglobal(L, funcName.c_str());
    if (!lua_isfunction(L, -1))
    {
        std::cerr << "'" << funcName << "' is not a function.\n";
        lua_pop(L, 1); // Remove non-function value
        return false;
    }

    // For simplicity, this example doesn't handle arguments and return values.
    // You can extend this method to push arguments and retrieve returns as needed.

    if (lua_pcall(L, args, returns, 0) != LUA_OK)
    {
        std::cerr << "Error calling '" << funcName << "': " << lua_tostring(L, -1) << "\n";
        lua_pop(L, 1); // Remove error message
        return false;
    }

    return true;
}

// Binding function to log messages from Lua
int lua_log_message(lua_State *L)
{
    // Check and retrieve the message string
    if (!lua_isstring(L, 1))
    {
        lua_pushstring(L, "Incorrect argument to 'Engine.Log'. Expected string as first argument.");
        lua_error(L);
        return 0; // Never reached, lua_error long jumps
    }
    const char *message = lua_tostring(L, 1);

    // Initialize default color
    ImVec4 color(1.0f, 1.0f, 1.0f, 1.0f); // Default white color

    // Check if a second argument (color) is provided
    if (lua_gettop(L) >= 2)
    {
        if (lua_istable(L, 2))
        {
            // Retrieve color components from the table
            lua_pushnumber(L, 1); // Push key 1 (r)
            lua_gettable(L, 2);
            float r = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : 1.0f;
            lua_pop(L, 1);

            lua_pushnumber(L, 2); // Push key 2 (g)
            lua_gettable(L, 2);
            float g = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : 1.0f;
            lua_pop(L, 1);

            lua_pushnumber(L, 3); // Push key 3 (b)
            lua_gettable(L, 2);
            float b = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : 1.0f;
            lua_pop(L, 1);

            lua_pushnumber(L, 4); // Push key 4 (a)
            lua_gettable(L, 2);
            float a = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : 1.0f;
            lua_pop(L, 1);

            color = ImVec4(r, g, b, a);
        }
        else
        {
            lua_pushstring(L, "Incorrect argument to 'Engine.Log'. Expected table for color.");
            lua_error(L);
            return 0;
        }
    }

    // Ensure LoggerWindow is valid

    g_LoggerWindow->AddLog(message, color);

    return 0; // Number of return values
}
