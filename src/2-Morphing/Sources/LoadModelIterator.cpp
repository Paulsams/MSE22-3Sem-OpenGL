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

    auto rootNode = SceneNode::create("Root");
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

        bufferViews_.insert({static_cast<int>(i), bufferView});
    }
}

std::shared_ptr<SceneNode> LoadModelIterator::createAndBindRenderersForModelNodes(tinygltf::Node &node) {
    auto currentRootNode = SceneNode::create(node.name);
    auto &transform = currentRootNode->getTransform();

    QMatrix4x4 offsetMatrix;
    if (node.matrix.size() == 16) {
        // Use `matrix' attribute
        transform.setModelMatrix(QMatrix4x4(
                node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3],
                node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7],
                node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11],
                node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15])
        );
    } else {
        offsetMatrix.setToIdentity();

        // Assume Trans x Rotate x Scale order
        if (node.translation.size() == 3) {
            transform.setPosition(node.translation[0], node.translation[1], node.translation[2]);
        }

        if (node.rotation.size() == 4) {
            transform.setRotation(QQuaternion(node.rotation[0], node.rotation[1],
                                              node.rotation[2], node.rotation[3]));
        }

        if (node.scale.size() == 3) {
            transform.setScale(node.scale[0], node.scale[1], node.scale[2]);
        }
    }

    if ((node.mesh >= 0) && (node.mesh < model_.meshes.size()))
        currentRootNode->addChild(createAndBindRenderersForMesh(model_.meshes[node.mesh]));

    for (int i: node.children)
        currentRootNode->addChild(createAndBindRenderersForModelNodes(model_.nodes[i]));

    return currentRootNode;
}

std::shared_ptr<SceneNode> LoadModelIterator::createAndBindRenderersForMesh(tinygltf::Mesh &mesh) {
    auto rootNodeForPrimitives = SceneNode::create(mesh.name);

    for (const auto &primitive: mesh.primitives) {
        std::shared_ptr<SceneNode> nodeForPrimitive = SceneNode::create("IDK need scene node");
        rootNodeForPrimitives->addChild(nodeForPrimitive);
        std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>(funcs_);

        tinygltf::Accessor indexAccessor = model_.accessors[primitive.indices];

        if (indexAccessor.bufferView == -1)
            continue;

        std::vector<std::pair<GLint, AttributeInfo>> attributes;

        for (auto &attrib: primitive.attributes) {
            tinygltf::Accessor accessor = model_.accessors[attrib.second];

            if (accessor.bufferView == -1)
                continue;

            tinygltf::BufferView &attributeBufferView = bufferViews_[accessor.bufferView];
            const tinygltf::Buffer &buffer = model_.buffers[attributeBufferView.buffer];

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
                                byteStride,
                                BufferData(&buffer.data.at(0) + attributeBufferView.byteOffset,
                                           static_cast<int>(attributeBufferView.byteLength))
                        ));
            } else {
                std::cout << "vaa missing: " << attrib.first << std::endl;
            }
        }

        ProgramInfoFromObject programInfoFromObject(program_, std::move(attributes));

        tinygltf::BufferView &indicesBufferBufferView = bufferViews_.at(indexAccessor.bufferView);
        const tinygltf::Buffer &indicesBuffer = model_.buffers[indicesBufferBufferView.buffer];

        BufferData indicesData(&indicesBuffer.data.at(0) + indicesBufferBufferView.byteOffset,
                               static_cast<int>(indicesBufferBufferView.byteLength));

        GLenum mode;
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
            continue;
        }

        IndicesInfo indicesInfo(mode,
                                static_cast<int>(indexAccessor.count),
                                indexAccessor.componentType,
                                BUFFER_OFFSET(indexAccessor.byteOffset));

        renderer->init(indicesInfo, indicesData, programInfoFromObject);

        nodeForPrimitive->changeRenderer(std::move(renderer));
    }

    return rootNodeForPrimitives;
}

