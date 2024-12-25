// src/UI.h
#pragma once

#include <GLFW/glfw3.h>
#include "ECS.h"
#include "TextureManager.h" // Include TextureManager.h

// Function declarations for UI panels
void ShowMainMenuBar();
void ShowViewport(TextureManager& textureManager, EntityManager& em, ComponentManager& cm);
void ShowConsole(bool* p_open);
void ShowEntityTree(EntityManager& em, ComponentManager& cm, Entity& selectedEntity);
void ShowInspector(EntityManager& em, ComponentManager& cm, Entity selectedEntity);
