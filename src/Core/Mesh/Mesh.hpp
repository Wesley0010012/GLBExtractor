#ifndef __MESH_H
#define __MESH_H

#include "../Config/MainConfig.hpp"

const float DEFAULT_TRANSFORM_SIZE = 1.0f;
const float DEFAULT_OBJECT_COLOR = 0.65f;

struct Mesh
{
    GLuint vao{}, vbo{}, ebo{};
    int indexCount{};
    glm::mat4 transform{DEFAULT_TRANSFORM_SIZE};
    glm::vec3 color{DEFAULT_OBJECT_COLOR};
};

#endif