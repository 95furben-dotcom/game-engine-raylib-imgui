#include "Components/Components.hpp"
#include <string>
#include <vector>
#pragma 
struct Object{
    std::string name;
    std::vector<Component*> components;

    Object(std::string name) : name(name){
        components.push_back(new CompTransform());
    }

};