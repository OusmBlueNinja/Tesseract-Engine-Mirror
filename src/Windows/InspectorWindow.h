#pragma once

#include <string>
#include <glm/glm.hpp> // or <glm/vec3.hpp> if you prefer
#include "imgui.h"

#include "Componenets/GameObject.h"
#include "Componenets/Mesh.h"
#include "Componenets/Transform.h"


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
    void Show();

private:

    // You can store additional state or styling here if needed
    // e.g. bool m_SomeInternalFlag = false;
};
