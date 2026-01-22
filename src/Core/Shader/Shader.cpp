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

static const char *fragmentSrc = R"(
#version 330 core
in vec3 Normal;
out vec4 FragColor;

uniform vec3 uColor;        
uniform vec3 uSunDir;
uniform vec3 uViewDir;

uniform float uMetallic;    
uniform float uRoughness;   
uniform float uClearCoat;   

void main()
{
    float uMetallic2 = 0.6f;
    float uRoughness2  = 0.18f;
    float uClearCoat2  = 1.0f;

    vec3 N = normalize(Normal);
    vec3 L = normalize(-uSunDir);
    vec3 V = normalize(uViewDir);
    vec3 H = normalize(L + V);

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = uColor * diff * 0.4;

    float shininess = mix(128.0, 16.0, uRoughness2);
    float spec = pow(max(dot(N, H), 0.0), shininess);

    vec3 metalSpec = mix(vec3(1.0), uColor, uMetallic2);
    vec3 specular = metalSpec * spec * 1.1;

    float fresnel = pow(1.0 - max(dot(N, V), 0.0), 5.0);
    float clearSpec = pow(max(dot(N, H), 0.0), 256.0);
    vec3 clearCoat = vec3(clearSpec) * fresnel * uClearCoat2;

    vec3 ambient = uColor * 0.65;

    vec3 color = ambient + diffuse + specular + clearCoat;
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
