#include "GLTFLoader.hpp"

static glm::mat4 getLocalMatrix(const tinygltf::Node &node)
{
    glm::mat4 m(1.0f);

    if (node.matrix.size() == 16)
    {
        m = glm::make_mat4(node.matrix.data());
    }
    else
    {
        if (node.translation.size() == 3)
            m = glm::translate(m, glm::vec3(
                                      node.translation[0],
                                      node.translation[1],
                                      node.translation[2]));

        if (node.rotation.size() == 4)
        {
            glm::quat q(
                node.rotation[3],
                node.rotation[0],
                node.rotation[1],
                node.rotation[2]);
            m *= glm::mat4_cast(q);
        }

        if (node.scale.size() == 3)
            m = glm::scale(m, glm::vec3(
                                  node.scale[0],
                                  node.scale[1],
                                  node.scale[2]));
    }
    return m;
}

static void processNode(
    const tinygltf::Model &model,
    int nodeIndex,
    const glm::mat4 &parentTransform,
    Scene &scene)
{
    const auto &node = model.nodes[nodeIndex];
    glm::mat4 globalTransform = parentTransform * getLocalMatrix(node);

    if (node.mesh >= 0)
    {
        const auto &mesh = model.meshes[node.mesh];

        for (const auto &prim : mesh.primitives)
        {

            if (!prim.attributes.count("POSITION") ||
                !prim.attributes.count("NORMAL") ||
                prim.indices < 0)
                continue;

            std::vector<float> vertices;
            std::vector<unsigned int> indices;

            const auto &posAcc = model.accessors[prim.attributes.at("POSITION")];
            const auto &posView = model.bufferViews[posAcc.bufferView];
            const auto &posBuf = model.buffers[posView.buffer];

            const auto &normAcc = model.accessors[prim.attributes.at("NORMAL")];
            const auto &normView = model.bufferViews[normAcc.bufferView];
            const auto &normBuf = model.buffers[normView.buffer];

            const float *posPtr = reinterpret_cast<const float *>(
                &posBuf.data[posView.byteOffset + posAcc.byteOffset]);

            const float *normPtr = reinterpret_cast<const float *>(
                &normBuf.data[normView.byteOffset + normAcc.byteOffset]);

            for (size_t i = 0; i < posAcc.count; i++)
            {
                glm::vec4 p = globalTransform * glm::vec4(
                                                    posPtr[i * 3 + 0],
                                                    posPtr[i * 3 + 1],
                                                    posPtr[i * 3 + 2],
                                                    1.0f);

                scene.bbMin = glm::min(scene.bbMin, glm::vec3(p));
                scene.bbMax = glm::max(scene.bbMax, glm::vec3(p));

                vertices.insert(vertices.end(), {posPtr[i * 3 + 0], posPtr[i * 3 + 1], posPtr[i * 3 + 2],
                                                 normPtr[i * 3 + 0], normPtr[i * 3 + 1], normPtr[i * 3 + 2]});
            }

            const auto &idxAcc = model.accessors[prim.indices];
            const auto &idxView = model.bufferViews[idxAcc.bufferView];
            const auto &idxBuf = model.buffers[idxView.buffer];

            const unsigned char *data =
                &idxBuf.data[idxView.byteOffset + idxAcc.byteOffset];

            for (size_t i = 0; i < idxAcc.count; i++)
            {
                unsigned int index = 0;

                switch (idxAcc.componentType)
                {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    index = reinterpret_cast<const uint8_t *>(data)[i];
                    break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    index = reinterpret_cast<const uint16_t *>(data)[i];
                    break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    index = reinterpret_cast<const uint32_t *>(data)[i];
                    break;
                default:
                    continue;
                }
                indices.push_back(index);
            }

            Mesh m;
            m.transform = globalTransform;
            m.indexCount = static_cast<int>(indices.size());

            glGenVertexArrays(1, &m.vao);
            glGenBuffers(1, &m.vbo);
            glGenBuffers(1, &m.ebo);

            glBindVertexArray(m.vao);

            glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
            glBufferData(
                GL_ARRAY_BUFFER,
                vertices.size() * sizeof(float),
                vertices.data(),
                GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                indices.size() * sizeof(unsigned int),
                indices.data(),
                GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            scene.meshes.push_back(m);
        }
    }

    for (int child : node.children)
        processNode(model, child, globalTransform, scene);
}

bool GLTFLoader::load(const std::string &file, Scene &scene)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    if (!loader.LoadBinaryFromFile(&model, &err, &warn, file))
        return false;

    const auto &scn = model.scenes[model.defaultScene];
    for (int node : scn.nodes)
        processNode(model, node, glm::mat4(1.0f), scene);

    return true;
}
