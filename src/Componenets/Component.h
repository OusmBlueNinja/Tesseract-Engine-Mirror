#pragma once

#include <string>
//#include <yaml-cpp/yaml.h>

class Component
{
public:
    virtual ~Component() {}
    virtual const std::string& GetName() const = 0;

    
    // Serialization methods
    //virtual YAML::Node Serialize() = 0;
    //virtual void Deserialize(const YAML::Node& node) = 0;
};