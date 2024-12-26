// src/Components/GameObject.h
#pragma once

#include <string>
#include "Transform.h"
#include "Mesh.h"


struct GameObject
{
    std::string name;    // Unique name for the GameObject
    Transform transform; // Position, Rotation, Scale
    Mesh mesh;           // Rendering Mesh
    // Add other components as needed
};
