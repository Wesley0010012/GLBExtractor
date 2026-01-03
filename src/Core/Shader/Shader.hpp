#ifndef __SHADER_H
#define __SHADER_H

#include "../../Config/MainConfig.hpp"

class Shader
{
public:
    static GLuint createDefault();

private:
    static GLuint compile(GLenum type, const char *src);
    static void checkCompile(GLuint shader);
    static void checkLink(GLuint program);
};

#endif