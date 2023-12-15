#include "LoaderModels/LoadModelIterator.h"
#include "SceneLogic/SceneNode.h"

#include <memory>
#include <iostream>
#include <vector>

#include <QOpenGLTexture>
#include <filesystem>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

std::shared_ptr<SceneNode> LoadModelIterator::create(const std::filesystem::path& path) {
    QImage defaultImage(1, 1,  QImage::Format_RGB32);
    defaultImage.fill(QColor(255,255,255,255));
    _defaultTexture = std::make_shared<QOpenGLTexture>(defaultImage);
    _defaultTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    _defaultTexture->setWrapMode(QOpenGLTexture::WrapMode::Repeat);

    for (const auto& textureInModel : model_.textures) {
        tinygltf::Image &image = model_.images[textureInModel.source];

        QOpenGLTexture* texture;

        if (!image.uri.empty()) {
            texture = new QOpenGLTexture(QImage((path.parent_path() / image.uri).string().c_str()));
            texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
            texture->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
        } else {
            // auto format = QOpenGLTexture::TextureFormat::RGBAFormat;
            //
            // // TODO: разные компоненты могут быть
            // if (image.component == 3)
            //     format = QOpenGLTexture::TextureFormat::RGBFormat;

            // GLenum type;
            // if (image.bits == 8) {
            //     type = GL_UNSIGNED_BYTE;
            // } else if (image.bits == 16) {
            //     type = GL_UNSIGNED_SHORT;
            // } else {
            //     // ???
            // }

            // TODO: не смог разобраться как без QImage. Хотя хочется, так как у него нет прямого сопоставления с OpenGL
            texture = new QOpenGLTexture(QImage(&image.image.at(0),
                image.width, image.height, QImage::Format_RGB32));
            texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
            texture->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
        }

        opengGLTextures_.push_back(std::shared_ptr<QOpenGLTexture>(texture));
    }

    const tinygltf::Scene &scene = model_.scenes[model_.defaultScene];
    for (int node: scene.nodes) {
        assert((node >= 0) && (node < model_.nodes.size()));
        initBufferForModelNodes(model_.nodes[node]);
    }

    auto rootNode = SceneNode::create(path.filename().replace_extension().string() + " Root");
    for (int node: scene.nodes)
        rootNode->addChild(createAndBindRenderersForModelNodes(model_.nodes[node]));

    return rootNode;
}

void LoadModelIterator::initBufferForModelNodes(tinygltf::Node &node) {
    if ((node.mesh >= 0) && (node.mesh < static_cast<int>(model_.meshes.size()))) {
        initBufferForMesh(model_.meshes[node.mesh]);
    }

    for (int i: node.children) {
        assert((i >= 0) && (i < model_.nodes.size()));
        initBufferForModelNodes(model_.nodes[i]);
    }
}

// TODO: вообще юзлесс -- можно убрать
void LoadModelIterator::initBufferForMesh(tinygltf::Mesh&) {
    for (size_t i = 0; i < model_.bufferViews.size(); ++i) {
        const tinygltf::BufferView &bufferView = model_.bufferViews[i];
        // TODO: Сделать drawArrays
        if (bufferView.target == 0) {
            std::cout << "WARN: check work drawArrays" << std::endl;
//            continue;  // Unsupported bufferView.
        }

        // TODO: хочется переиспользовать буфера, если они одинаковые юзаются
        bufferViews_.insert({static_cast<int>(i), bufferView});
    }
}

std::shared_ptr<SceneNode> LoadModelIterator::createAndBindRenderersForModelNodes(const tinygltf::Node &node) {
    auto currentRootNode = SceneNode::create(node.name);
    auto &transform = currentRootNode->getTransform();

    QMatrix4x4 offsetMatrix;
    if (node.matrix.size() == 16) {
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

    for (const int i: node.children)
        currentRootNode->addChild(createAndBindRenderersForModelNodes(model_.nodes[i]));

    return currentRootNode;
}

std::shared_ptr<SceneNode> LoadModelIterator::createAndBindRenderersForMesh(tinygltf::Mesh &mesh) {
    auto rootNodeForPrimitives = SceneNode::create(mesh.name);

    // TODO: на модели chess почему-то BlackDrawer и WhiteDrawer входят в друг-друга
    for (const auto& primitive: mesh.primitives) {
        // Нет смысла делать отдельными нодами
        std::shared_ptr<SceneNode> nodeForPrimitive = SceneNode::create("Sub Mesh");
        rootNodeForPrimitives->addChild(nodeForPrimitive);
        std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>(funcs_);

        std::vector<std::pair<GLint, AttributeInfo>> attributes;

        int countVertices = -1;
        for (auto &attrib: primitive.attributes) {
            tinygltf::Accessor accessor = model_.accessors[attrib.second];

            if (accessor.bufferView == -1)
                continue;

            tinygltf::BufferView &attributeBufferView = bufferViews_[accessor.bufferView];
            const tinygltf::Buffer &buffer = model_.buffers[attributeBufferView.buffer];

            int byteStride =
                    accessor.ByteStride(model_.bufferViews[accessor.bufferView]);

            int sizeType = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                sizeType = accessor.type;
            }

            int vaa = -1;
            if (attrib.first == "POSITION") {
                vaa = 0;
                countVertices = attributeBufferView.byteLength / tinygltf::GetComponentSizeInBytes(accessor.componentType);
            }
            if (attrib.first == "NORMAL") vaa = 1;
            if (attrib.first == "TEXCOORD_0") vaa = 2;
            if (vaa > -1) {
                attributes.emplace_back(
                        vaa,
                        AttributeInfo(
                                accessor.componentType,
                                static_cast<int>(accessor.byteOffset),
                                sizeType,
                                byteStride,
                                BufferData(&buffer.data.at(0) + attributeBufferView.byteOffset,
                                           static_cast<int>(attributeBufferView.byteLength))
                        ));
            } else {
                std::cout << "vaa missing: " << attrib.first << std::endl;
            }
        }

        if (countVertices == -1)
            continue;

        ProgramInfoFromObject programInfoFromObject(program_, std::move(attributes));

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

        tinygltf::Accessor indexAccessor = model_.accessors[primitive.indices];
        std::optional<IndicesBuffer> indicesBufferContainer = std::nullopt;
        if (indexAccessor.bufferView != -1) {
            tinygltf::BufferView& indicesBufferBufferView = bufferViews_.at(indexAccessor.bufferView);
            const tinygltf::Buffer& indicesBuffer = model_.buffers[indicesBufferBufferView.buffer];

            if (indicesBufferBufferView.target != 0) {
                indicesBufferContainer = IndicesBuffer(
                        IndicesInfo(
                                mode,
                                static_cast<int>(indexAccessor.count),
                                indexAccessor.componentType,
                                BUFFER_OFFSET(indexAccessor.byteOffset)
                        ),
                        BufferData(
                                &indicesBuffer.data.at(0) + indicesBufferBufferView.byteOffset,
                                static_cast<int>(indicesBufferBufferView.byteLength)
                        )
                );
            }
        }

        Renderer::TexturesContainer textures;
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
            QVector3D diffuseColor = {
                static_cast<float>(material.pbrMetallicRoughness.baseColorFactor[0]),
                static_cast<float>(material.pbrMetallicRoughness.baseColorFactor[1]),
                static_cast<float>(material.pbrMetallicRoughness.baseColorFactor[2])
            };

            textures.push_back({
                TextureType::Diffuse,
                {
                    diffuseColor,
                    material.pbrMetallicRoughness.baseColorTexture.index > -1
                    ? opengGLTextures_[material.pbrMetallicRoughness.baseColorTexture.index]
                    : _defaultTexture,
                }
            });
        }

        renderer->init(mode, countVertices, programInfoFromObject, indicesBufferContainer, std::move(textures));

        nodeForPrimitive->changeRenderer(std::move(renderer));
    }

    return rootNodeForPrimitives;
}

