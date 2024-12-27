#pragma once

#include "lua.hpp"
#include <string>

class LuaManager {
public:
    LuaManager();
    ~LuaManager();

    // Initialize the Lua state and load the script
    bool init(const std::string& scriptPath);

    // Call the 'onUpdate' function in Lua
    bool onUpdate(float deltaTime);

    // Call the 'onDrawGui' function in Lua
    bool onDrawGui();

    // Optionally, call other Lua functions as needed
    bool callFunction(const std::string& funcName, int args = 0, int returns = 0);

private:
    lua_State* L;
        std::string m_LastErrorMessage; // Stores the last error message


    // Helper function to call a Lua function with no arguments and no return values

    bool callLuaFunction(const std::string& funcName);
};

