// GameObject.h
#pragma once
#include "Transform.h"
#include "Mesh.h"

// A game object that has a transform and a mesh
struct GameObject
{
    Transform transform;
    Mesh mesh;
};