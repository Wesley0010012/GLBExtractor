#ifndef __RENDERER_H
#define __RENDERER_H

#include "../../Config/MainConfig.hpp"

#include "../Scene/Scene.hpp"
#include "../Shader/Shader.hpp"
#include "../ColoredMesh/ColoredMesh.hpp"

class Renderer
{
public:
    void init();
    void render(
        Scene &scene,
        const glm::mat4 &view,
        const glm::mat4 &projection,
        std::vector<ColoredMesh> meshes,
        glm::vec3 sunDirection);

private:
    unsigned int program{};
};

#endif