#include "./Renderer.hpp"

void Renderer::init()
{
    program = Shader::createDefault();
}

void Renderer::render(
    Scene &scene,
    const glm::mat4 &view,
    const glm::mat4 &projection,
    std::vector<ColoredMesh> meshes,
    glm::vec3 sunDirection)
{
    glUseProgram(program);

    GLint sunLoc = glGetUniformLocation(program, "uSunDir");

    glUniform3fv(sunLoc, 1, &sunDirection[0]);

    for (int i = 0; i < scene.meshes.size(); i++)
    {
        auto &m = scene.meshes[i];

        m.color = glm::vec3(0.65f);

        for (int j = 0; j < meshes.size(); j++)
        {
            if (meshes[j].meshNumber == i)
            {
                m.color = glm::vec3(meshes[j].r, meshes[j].g, meshes[j].b);
            }
        }

        glUniformMatrix4fv(
            glGetUniformLocation(program, "uModel"),
            1, GL_FALSE, glm::value_ptr(m.transform));

        glUniformMatrix4fv(
            glGetUniformLocation(program, "uView"),
            1, GL_FALSE, glm::value_ptr(view));

        glUniformMatrix4fv(
            glGetUniformLocation(program, "uProjection"),
            1, GL_FALSE, glm::value_ptr(projection));

        glUniform3fv(
            glGetUniformLocation(program, "uColor"),
            1, glm::value_ptr(m.color));

        glBindVertexArray(m.vao);
        glDrawElements(GL_TRIANGLES, m.indexCount, GL_UNSIGNED_INT, 0);
    }
}
