#include "./Shader.hpp"
#include <iostream>

static const char *vertexSrc = R"(#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 Normal;

void main() {
    Normal = mat3(transpose(inverse(uModel))) * aNormal;
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}
)";

static const char *fragmentSrc = R"(#version 330 core
in vec3 Normal;
out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uSunDir;

void main() {
    vec3 N = normalize(Normal);
    vec3 L = normalize(-uSunDir);

    float diff = max(dot(N, L), 0.0);

    vec3 color = uColor * (0.25 + diff * 0.75);
    FragColor = vec4(color, 1.0);
}
)";

GLuint Shader::compile(GLenum type, const char *src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    checkCompile(shader);
    return shader;
}

void Shader::checkCompile(GLuint shader)
{
    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compile error:\n"
                  << log << std::endl;
    }
}

void Shader::checkLink(GLuint program)
{
    GLint ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[1024];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "Program link error:\n"
                  << log << std::endl;
    }
}

GLuint Shader::createDefault()
{
    GLuint vs = compile(GL_VERTEX_SHADER, vertexSrc);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fragmentSrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    checkLink(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
