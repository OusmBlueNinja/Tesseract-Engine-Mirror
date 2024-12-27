#include "SceneManager.h"

#include "./Componenets/Component.h"
#include "./Componenets/Transform.h"
#include "./Componenets/Mesh.h"
#include "./Componenets/GameObject.h"

#include "imgui.h"

#include "./Windows/LoggerWindow.h"



#include <yaml-cpp/yaml.h>
#include <fstream>
#include <filesystem>

extern LoggerWindow *g_LoggerWindow;




void SceneManager::SaveScene(const std::vector<std::shared_ptr<GameObject>> &gameobjects, const std::string &filename)
{
    YAML::Node sceneNode;

    for (const auto &gameobject : gameobjects)
    {
        sceneNode["Entities"].push_back(gameobject->Serialize());
    }

    std::ofstream fout(filename);
    fout << sceneNode;
}

void SceneManager::LoadScene(std::vector<std::shared_ptr<GameObject>> &gameobjects, const std::string &filename)
{
    if (!std::filesystem::exists(filename) || !std::filesystem::is_regular_file(filename)) {

        g_LoggerWindow->AddLog("Error: File not found: %s", ImVec4(1.0f,0.0f,0.0f,1.0f), filename.c_str());
        return;
    }



    YAML::Node sceneNode = YAML::LoadFile(filename);
    gameobjects.clear();

    if (sceneNode["Entities"])
    {
        for (const auto &gameobjectNode : sceneNode["Entities"])
        {
            int id = gameobjectNode["ID"].as<int>();
            std::string name = gameobjectNode["Name"].as<std::string>();
            auto gameobject = std::make_shared<GameObject>(id, name);
            gameobject->Deserialize(gameobjectNode);
            gameobjects.push_back(gameobject);
        }
    }
}