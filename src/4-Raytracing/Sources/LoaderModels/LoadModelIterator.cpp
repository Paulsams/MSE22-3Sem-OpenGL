#include "LoaderModels/LoadModelIterator.h"
#include "Materials/material.h"
#include "glm/vec4.hpp"

#include <memory>
#include <iostream>
#include <vector>

#include <filesystem>

std::vector<std::shared_ptr<model>> LoadModelIterator::create(vec3 position, const std::filesystem::path& path) {
    const tinygltf::Scene &scene = model_.scenes[model_.defaultScene];

    glm::mat4 modelMatrix(1.0);
    modelMatrix = glm::translate(modelMatrix, {position.x(), position.y(), position.z()});
    for (int node: scene.nodes)
        createAndBindRenderersForModelNodes(model_.nodes[node], modelMatrix);

    return models_;
}

void LoadModelIterator::createAndBindRenderersForModelNodes(const tinygltf::Node &node, glm::mat4 modelMatrix) {
    glm::mat4 offsetMatrix(1.0);
    if (node.matrix.size() == 16) {
        offsetMatrix = glm::mat4x4 (
                node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3],
                node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7],
                node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11],
                node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15]
        );
    } else {
        // Assume Trans x Rotate x Scale order
        if (node.translation.size() == 3) {
            offsetMatrix = glm::translate(offsetMatrix, {node.translation[0], node.translation[1], node.translation[2]});
        }

        if (node.rotation.size() == 4) {
            auto rotation = glm::quat(
                    static_cast<float>(node.rotation[0]),
                    static_cast<float>(node.rotation[1]),
                    static_cast<float>(node.rotation[2]),
                    static_cast<float>(node.rotation[3])
            );

            offsetMatrix = glm::rotate(offsetMatrix, angle(rotation), axis(rotation));
        }

        if (node.scale.size() == 3) {
            offsetMatrix = glm::scale(offsetMatrix, {node.scale[0], node.scale[1], node.scale[2]});
        }
    }
    modelMatrix *= offsetMatrix;

    if ((node.mesh >= 0) && (static_cast<size_t>(node.mesh) < model_.meshes.size()))
        createAndBindRenderersForMesh(model_.meshes[node.mesh], modelMatrix);

    for (const int i: node.children)
        createAndBindRenderersForModelNodes(model_.nodes[i], modelMatrix);
}

void LoadModelIterator::createAndBindRenderersForMesh(tinygltf::Mesh &mesh, const glm::mat4x4& modelMatrix) {
    for (const auto& primitive: mesh.primitives) {
        std::vector<vec3> points;
        std::vector<vec3> normals;
        std::vector<unsigned short> indices;

        size_t countVertices = 0;
        for (auto& attrib: primitive.attributes) {
            tinygltf::Accessor accessor = model_.accessors[attrib.second];

            if (accessor.bufferView == -1)
                continue;

            tinygltf::BufferView &attributeBufferView = model_.bufferViews[accessor.bufferView];
            const tinygltf::Buffer &buffer = model_.buffers[attributeBufferView.buffer];

            int byteStride =
                    accessor.ByteStride(model_.bufferViews[accessor.bufferView]);

            int vaa = -1;
            std::vector<vec3>* attributeBufferData;
            if (attrib.first == "POSITION") {
                vaa = 0;
                attributeBufferData = &points;
                countVertices = attributeBufferView.byteLength /
                        static_cast<size_t>(tinygltf::GetComponentSizeInBytes(accessor.componentType));
            }
            if (attrib.first == "NORMAL") {
                vaa = 1;
                attributeBufferData = &normals;
            }

            if (vaa > -1) {
                if (accessor.type != TINYGLTF_TYPE_VEC3) {
                    assert(0);
                    continue;
                }

                if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
                    assert(0);
                    continue;
                }

                for (size_t i = 0; i < attributeBufferView.byteLength; i += byteStride) {
                    size_t index = accessor.byteOffset + i;
                    vec3 item = vec3(modelMatrix * glm::vec4(
                            *((float*)(buffer.data.data() + index) + 0),
                            *((float*)(buffer.data.data() + index) + 1),
                            *((float*)(buffer.data.data() + index) + 2),
                            1.0f
                    ));
                    attributeBufferData->push_back(item);
                }
            }
        }

        if (countVertices == 0)
            continue;

        if (primitive.mode != TINYGLTF_MODE_TRIANGLES) {
            assert(0);
            continue;
        }

        tinygltf::Accessor indexAccessor = model_.accessors[primitive.indices];
        if (indexAccessor.bufferView != -1) {
            tinygltf::BufferView& indicesBufferBufferView = model_.bufferViews[indexAccessor.bufferView];
            const tinygltf::Buffer& indicesBuffer = model_.buffers[indicesBufferBufferView.buffer];

            if (indexAccessor.componentType != TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                assert(0);
                continue;
            }

            if (indicesBufferBufferView.target != 0) {
                for (size_t i = 0; i < indicesBufferBufferView.byteLength; i += sizeof(unsigned short)) {
                    size_t index = indicesBufferBufferView.byteOffset + i;
                    indices.push_back(*(unsigned short*)(indicesBuffer.data.data() + index));
                }
            }
        }

        std::shared_ptr<material> model_material;
        if (primitive.material >= 0) {
            tinygltf::Material &material = model_.materials[primitive.material];

            // model_.material.base_color = { material.pbrMetallicRoughness.baseColorFactor[0]
            //                             , material.pbrMetallicRoughness.baseColorFactor[1]
            //                             , material.pbrMetallicRoughness.baseColorFactor[2] };
            // model_.material.metallic = material.pbrMetallicRoughness.metallicFactor;
            // model_.material.roughness = material.pbrMetallicRoughness.roughnessFactor;
            // model_.material.emitted = { material.emissiveFactor[0]
            //                          , material.emissiveFactor[1]
            //                          , material.emissiveFactor[2] };

            // TODO: что-то кроме диффуза
            vec3 diffuseColor = {
                    static_cast<float>(material.pbrMetallicRoughness.baseColorFactor[0]),
                    static_cast<float>(material.pbrMetallicRoughness.baseColorFactor[1]),
                    static_cast<float>(material.pbrMetallicRoughness.baseColorFactor[2])
            };

            model_material = std::make_shared<lambertian>(diffuseColor);
        } else {
            model_material = std::make_shared<lambertian>(vec3{0.2, 0.2, 0.2});
        }

        models_.push_back(std::make_shared<model>(
                std::move(points), std::move(normals),
                std::move(indices), model_material)
        );
    }
}

