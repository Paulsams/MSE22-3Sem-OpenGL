#include <memory>
#include <iostream>

#include "LoadModelIterator.h"
#include "SceneNode.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

std::shared_ptr<SceneNode> LoadModelIterator::create() {
    const tinygltf::Scene &scene = model_.scenes[model_.defaultScene];
    for (int node: scene.nodes) {
        assert((node >= 0) && (node < model_.nodes.size()));
        initBufferForModelNodes(model_.nodes[node]);
    }

    auto rootNode = std::make_shared<SceneNode>("Root");
    for (int node: scene.nodes)
        rootNode->addChild(createAndBindRenderersForModelNodes(model_.nodes[node]));

    return rootNode;
}

void LoadModelIterator::initBufferForModelNodes(tinygltf::Node &node) {
    if ((node.mesh >= 0) && (node.mesh < model_.meshes.size())) {
        initBufferForMesh(model_.meshes[node.mesh]);
    }

    for (int i: node.children) {
        assert((i >= 0) && (i < model_.nodes.size()));
        initBufferForModelNodes(model_.nodes[i]);
    }
}

void LoadModelIterator::initBufferForMesh(tinygltf::Mesh &) {
    for (size_t i = 0; i < model_.bufferViews.size(); ++i) {
        const tinygltf::BufferView &bufferView = model_.bufferViews[i];
        if (bufferView.target == 0) {
            std::cout << "WARN: bufferView.target is zero" << std::endl;
            continue;  // Unsupported bufferView.
        }

        const tinygltf::Buffer &buffer = model_.buffers[bufferView.buffer];
        std::cout << "bufferview.target " << bufferView.target << std::endl;

        buffers_.insert({i, buffer});
    }
}

std::shared_ptr<SceneNode> LoadModelIterator::createAndBindRenderersForModelNodes(tinygltf::Node &node) {
    auto currentRootNode = std::make_shared<SceneNode>(node.name);

    if ((node.mesh >= 0) && (node.mesh < model_.meshes.size()))
        currentRootNode->addChild(createAndBindRenderersForMesh(model_.meshes[node.mesh]));

    for (int i: node.children)
        currentRootNode->addChild(createAndBindRenderersForModelNodes(model_.nodes[i]));

    return currentRootNode;
}

std::shared_ptr<SceneNode> LoadModelIterator::createAndBindRenderersForMesh(tinygltf::Mesh &mesh) {
    auto rootNodeForPrimitives = std::make_shared<SceneNode>(mesh.name);

    for (const auto &primitive: mesh.primitives) {
        std::shared_ptr<SceneNode> nodeForPrimitive = std::make_shared<SceneNode>("IDK need scene node");
        rootNodeForPrimitives->addChild(nodeForPrimitive);
        std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>(funcs_);

        tinygltf::Accessor indexAccessor = model_.accessors[primitive.indices];

        if (indexAccessor.bufferView == -1)
            continue;

        std::vector<BufferData> verticesData;
        std::vector<std::pair<GLint, AttributeInfo>> attributes;

        for (auto &attrib: primitive.attributes) {
            tinygltf::Accessor accessor = model_.accessors[attrib.second];

            auto attributeBuffer = buffers_[accessor.bufferView];
            const tinygltf::BufferView &bufferView = model_.bufferViews[accessor.bufferView];
            verticesData.push_back(BufferData(&attributeBuffer.data.at(0) + bufferView.byteOffset,
                                   static_cast<int>(bufferView.byteLength)));

            if (accessor.bufferView == -1)
                continue;

            int byteStride =
                    accessor.ByteStride(model_.bufferViews[accessor.bufferView]);

            int size = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                size = accessor.type;
            }

            int vaa = -1;
            if (attrib.first == "POSITION") vaa = 0;
            if (attrib.first == "NORMAL") vaa = 1;
            if (attrib.first == "TEXCOORD_0") vaa = 2;
            if (vaa > -1) {
                attributes.emplace_back(
                        vaa,
                        AttributeInfo(
                                accessor.componentType,
                                static_cast<int>(accessor.byteOffset),
                                size,
                                byteStride
                        ));
            } else {
                std::cout << "vaa missing: " << attrib.first << std::endl;
            }
        }

        ProgramInfoFromObject programInfoFromObject(program_, std::move(attributes));

        tinygltf::Buffer &indecesBuffer = buffers_.at(indexAccessor.bufferView);
        const tinygltf::BufferView &bufferView = model_.bufferViews[indexAccessor.bufferView];

        BufferData indecesData(&indecesBuffer.data.at(0) + bufferView.byteOffset,
                               static_cast<int>(bufferView.byteLength));

        GLenum mode = -1;
        if (primitive.mode == TINYGLTF_MODE_TRIANGLES) {
            mode = GL_TRIANGLES;
        } else if (primitive.mode == TINYGLTF_MODE_TRIANGLE_STRIP) {
            mode = GL_TRIANGLE_STRIP;
        } else if (primitive.mode == TINYGLTF_MODE_TRIANGLE_FAN) {
            mode = GL_TRIANGLE_FAN;
        } else if (primitive.mode == TINYGLTF_MODE_POINTS) {
            mode = GL_POINTS;
        } else if (primitive.mode == TINYGLTF_MODE_LINE) {
            mode = GL_LINES;
        } else if (primitive.mode == TINYGLTF_MODE_LINE_LOOP) {
            mode = GL_LINE_LOOP;
        } else {
            assert(0);
        }

        IndicesInfo indicesInfo(mode,
                                static_cast<int>(indexAccessor.count),
                                indexAccessor.componentType,
                                BUFFER_OFFSET(indexAccessor.byteOffset));

        renderer->init(indicesInfo, verticesData, indecesData, programInfoFromObject);

        nodeForPrimitive->changeRenderer(std::move(renderer));
    }

    return rootNodeForPrimitives;
}

