#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "Componenets/GameObject.h"


// SceneWindow class declaration
class SceneWindow {
public:
    void Show();
    GameObject* GetSelectedObject() const;

private:
    void AddGameObject();      // Adds a new game object
    void RemoveGameObject(int index); // Removes a game object by index
};

