#ifndef __SCENE_H
#define __SCENE_H

#include "../../Config/MainConfig.hpp"
#include "../Mesh/Mesh.hpp"

struct Scene
{
    std::vector<Mesh> meshes;
    glm::vec3 bbMin{FLT_MAX};
    glm::vec3 bbMax{-FLT_MAX};
};

#endif