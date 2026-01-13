#include "../Config/MainConfig.hpp"
#include "../Core/Scene/Scene.hpp"
#include "../Core/Renderer/Renderer.hpp"
#include "../Core/GLTFLoader/GLTFLoader.hpp"
#include "../Core/ImageWriter/ImageWriter.hpp"
#include "../Core/ColoredMesh/ColoredMesh.hpp"

using json = nlohmann::json;

inline void from_json(const json &j, ColoredMesh &cm)
{
    j.at("r").get_to(cm.r);
    j.at("g").get_to(cm.g);
    j.at("b").get_to(cm.b);
    j.at("meshName").get_to(cm.meshName);
}

int main(int argc, char **argv)
{
    json j = json::parse(argv[1]);

    std::string modelPath = std::string(argv[2]);
    std::string outputPath = std::string(argv[3]);
    float width = std::atof(argv[4]);
    float height = std::atof(argv[5]);

    float camX = std::atof(argv[6]);
    float camY = std::atof(argv[7]);
    float camZ = std::atof(argv[8]);

    std::vector<ColoredMesh> meshes = j.get<std::vector<ColoredMesh>>();

    if (!glfwInit())
    {
        std::cerr << "GLFW init failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow *window = glfwCreateWindow(
        width,
        height,
        "offscreen",
        nullptr,
        nullptr);

    if (!window)
    {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    int ih = (int)height;
    int iw = (int)width;

    glfwGetFramebufferSize(window, &iw, &ih);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "GLAD init failed\n";
        return -1;
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    Scene scene;

    if (!GLTFLoader::load(modelPath, scene))
    {
        std::cerr << "Failed to load GLTF\n";
        return -1;
    }

    glm::vec3 center = (scene.bbMin + scene.bbMax) * 0.5f;
    float radius = glm::length(scene.bbMax - scene.bbMin) * 0.5f;

    float aspect = static_cast<float>(width) / height;
    float fovY = glm::radians(FOV_Y);

    float distance = (radius / std::sin(fovY * 0.9f)) * CAMERA_PADDING;

    glm::vec3 cameraDir = glm::normalize(glm::vec3(camX, camY, camZ));
    glm::vec3 cameraPos = center + cameraDir * distance;

    glm::vec3 sunDirection = glm::normalize(glm::vec3(0.0f) - cameraPos);

    glm::mat4 view = glm::lookAt(
        cameraPos,
        center,
        glm::vec3(0, 1, 0));

    float nearPlane = std::max(distance - radius * 2.0f, 0.05f);
    float farPlane = distance + radius * 3.0f;

    glm::mat4 projection = glm::perspective(
        fovY,
        aspect,
        nearPlane,
        farPlane);

    Renderer renderer;
    renderer.init();

    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderer.render(scene, view, projection, meshes, sunDirection);

    ImageWriter::savePNG(
        outputPath,
        width,
        height);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
