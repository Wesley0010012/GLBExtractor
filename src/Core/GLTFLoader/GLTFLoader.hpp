#ifndef __GLTF_LOADER_H
#define __GLTF_LOADER_H

#include "../../Config/MainConfig.hpp"
#include "../Scene/Scene.hpp"

class GLTFLoader
{
public:
    static glm::mat4 getLocalMatrix(const tinygltf::Node &node);

    static bool load(const std::string &file, Scene &scene);
};

#endif