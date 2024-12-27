#include "GameObject.h"




class SceneManager
{
public:
    void SaveScene(const std::vector<std::shared_ptr<GameObject>> &gameobjects, const std::string &filename);
    void LoadScene(std::vector<std::shared_ptr<GameObject>> &gameobjects, const std::string &filename);

private:
};