// LuaMacros.h

#pragma once

#include "lua.hpp"

// Macro to create a Lua table with a specified name
#define CREATE_LUA_TABLE(L, tableName)  \
    lua_newtable(L);                     \
    int tableName##_index = lua_gettop(L); \
    lua_setglobal(L, #tableName);        \
    lua_getglobal(L, #tableName);        \
    tableName##_index = lua_gettop(L);

// Macro to bind a C++ function to a Lua table
#define BIND_LUA_FUNCTION(L, tableName, funcName, cFunc) \
    lua_pushcfunction(L, cFunc);                         \
    lua_setfield(L, tableName##_index, funcName);

// Macro to define and bind a Lua function within a table
#define DEFINE_AND_BIND_LUA_FUNCTION(L, tableName, funcName, cFunc) \
    lua_pushcfunction(L, cFunc);                                    \
    lua_setfield(L, tableName##_index, funcName);

// Macro to register multiple functions to a Lua table
#define REGISTER_LUA_FUNCTIONS(L, tableName, ...) \
    do {                                          \
        const char* funcs[] = {__VA_ARGS__};      \
        for (const char* func : funcs) {          \
            /* Assume corresponding C function has prefix lua_ and tableName_ */ \
            std::string cFuncName = std::string("lua_") + tableName + "_" + func; \
            lua_pushcfunction(L, (lua_CFunction)(&cFuncName[0])); /* Needs appropriate mapping */ \
            lua_setfield(L, tableName##_index, func); \
        }                                         \
    } while(0)


// Macro to get an integer from Lua
#define GET_LUA_INT(L, index, varName)                     \
    int varName = 0;                                        \
    if (lua_isinteger(L, index)) {                         \
        varName = static_cast<int>(lua_tointeger(L, index)); \
    } else {                                                \
        std::cerr << "Expected integer at argument " << index << "\n"; \
    }

// Macro to get a float from Lua
#define GET_LUA_FLOAT(L, index, varName)                   \
    float varName = 0.0f;                                   \
    if (lua_isnumber(L, index)) {                           \
        varName = static_cast<float>(lua_tonumber(L, index)); \
    } else {                                                \
        std::cerr << "Expected number at argument " << index << "\n"; \
    }

// Macro to get a string from Lua
#define GET_LUA_STRING(L, index, varName)                  \
    std::string varName;                                     \
    if (lua_isstring(L, index)) {                           \
        varName = lua_tostring(L, index);                   \
    } else {                                                \
        std::cerr << "Expected string at argument " << index << "\n"; \
    }

// Macro to get a boolean from Lua
#define GET_LUA_BOOL(L, index, varName)                    \
    bool varName = false;                                    \
    if (lua_isboolean(L, index)) {                          \
        varName = lua_toboolean(L, index);                  \
    } else {                                                \
        std::cerr << "Expected boolean at argument " << index << "\n"; \
    }

// Macro to return a value to Lua
#define RETURN_LUA_VALUE(L, value)                          \
    lua_push##value(L, value);                               \
    return 1;

// Examples for returning integer, float, string, boolean
#define RETURN_LUA_INT(L, value) RETURN_LUA_VALUE(L, integer)
#define RETURN_LUA_FLOAT(L, value) RETURN_LUA_VALUE(L, number)
#define RETURN_LUA_STRING(L, value) RETURN_LUA_VALUE(L, string)
#define RETURN_LUA_BOOL(L, value) RETURN_LUA_VALUE(L, boolean)



#define RETURN_LUA_VALUE(L, value)                          \
    lua_push##value(L, value);                               \
    return 1;

// Specific return macros
#define RETURN_LUA_INT(L, value) RETURN_LUA_VALUE(L, integer)
#define RETURN_LUA_FLOAT(L, value) RETURN_LUA_VALUE(L, number)
#define RETURN_LUA_STRING(L, value) RETURN_LUA_VALUE(L, string)
#define RETURN_LUA_BOOL(L, value) RETURN_LUA_VALUE(L, boolean)