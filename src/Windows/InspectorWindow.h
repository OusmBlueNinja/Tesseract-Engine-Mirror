#pragma once

#include <string>
#include <glm/glm.hpp> // or <glm/vec3.hpp> if you prefer
#include "imgui.h"

// Example struct for a Transform component
struct Transform
{
    float position[3] = {0.0f, 0.0f, 0.0f};
    float rotation[3] = {0.0f, 0.0f, 0.0f};
    float scale[3]    = {1.0f, 1.0f, 1.0f};
};

// Example struct for a Script component
struct Script
{
    std::string scriptName = "MyBehavior.lua";
    bool enabled = true;
};

// The Inspector window class
class InspectorWindow
{
public:
    // Constructor / Destructor
    InspectorWindow() = default;
    ~InspectorWindow() = default;

    // Show the window (call each frame)
    // Pass references to your components, so any changes get applied to them.
    void Show(Transform& transform, Script& script);

private:
    // You can store additional state or styling here if needed
    // e.g. bool m_SomeInternalFlag = false;
};
