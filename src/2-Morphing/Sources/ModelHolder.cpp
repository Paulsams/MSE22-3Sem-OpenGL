#include <iostream>
#include "ModelHolder.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

ModelHolder::ModelHolder(const char *filename, QOpenGLFunctions &glFuncs, QOpenGLShaderProgram &program)
        : model_(loadModel(filename)), funcs_(glFuncs), program_(program) {}

void ModelHolder::init() {
    bindModel();
}

void ModelHolder::draw() {
    drawModel();
}

std::unique_ptr<tinygltf::Model> ModelHolder::loadModel(const char *filename) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
    if (!warn.empty())
        std::cout << "WARN: " << warn << std::endl;

    if (!err.empty())
        std::cout << "ERR: " << err << std::endl;

    if (!res)
        std::cout << "Failed to load glTF: " << filename << std::endl;
    else
        std::cout << "Loaded glTF: " << filename << std::endl;

    return std::make_unique<tinygltf::Model>(model);
}

void ModelHolder::bindModel() {
    vao_.create();
    vao_.bind();

    const tinygltf::Scene &scene = model_->scenes[model_->defaultScene];
    for (int node: scene.nodes) {
        assert((node >= 0) && (node < model_->nodes.size()));
        bindModelNodes(model_->nodes[node]);
    }

//    glBindVertexArray(0);
//    // cleanup vbos but do not delete index buffers yet
//    for (auto it = vbos.cbegin(); it != vbos.cend();) {
//        tinygltf::BufferView bufferView = model.bufferViews[it->first];
//        if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
//            glDeleteBuffers(1, &vbos[it->first]);
//            vbos.erase(it++);
//        }
//        else {
//            ++it;
//        }
//    }

    vao_.release();
}

void ModelHolder::bindModelNodes(tinygltf::Node &node) {
    if ((node.mesh >= 0) && (node.mesh < model_->meshes.size())) {
        bindMesh(model_->meshes[node.mesh]);
    }

    for (int i: node.children) {
        assert((i >= 0) && (i < model_->nodes.size()));
        bindModelNodes(model_->nodes[i]);
    }
}

void ModelHolder::bindMesh(tinygltf::Mesh &mesh) {
    for (size_t i = 0; i < model_->bufferViews.size(); ++i) {
        const tinygltf::BufferView &bufferView = model_->bufferViews[i];
        if (bufferView.target == 0) {  // TODO impl drawarrays
            std::cout << "WARN: bufferView.target is zero" << std::endl;
            continue;  // Unsupported bufferView.
            /*
              From spec2.0 readme:
              https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
                       ... drawArrays function should be used with a count equal to
              the count            property of any of the accessors referenced by the
              attributes            property            (they are all equal for a given
              primitive).
            */
        }

        const tinygltf::Buffer &buffer = model_->buffers[bufferView.buffer];
        std::cout << "bufferview.target " << bufferView.target << std::endl;

        QOpenGLBuffer vbo{QOpenGLBuffer::Type::VertexBuffer};
        vbo.create();
        vbo.bind();
        vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
        // Как-нибудь заюзать bufferView.target мейби надо?

        std::cout << "buffer.data.size = " << buffer.data.size()
                  << ", bufferview.byteOffset = " << bufferView.byteOffset
                  << std::endl;

        vbo.allocate(&buffer.data.at(0) + bufferView.byteOffset,
                     static_cast<int>(bufferView.byteLength));
//        glBufferData(bufferView.target, bufferView.byteLength,
//                     &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);

        vbo.release();
        vbos_[i] = vbo;
    }

    for (const auto &primitive: mesh.primitives) {
        tinygltf::Accessor indexAccessor = model_->accessors[primitive.indices];

        for (auto &attrib: primitive.attributes) {
            tinygltf::Accessor accessor = model_->accessors[attrib.second];
            int byteStride =
                    accessor.ByteStride(model_->bufferViews[accessor.bufferView]);

            QOpenGLBuffer& vbo = vbos_[accessor.bufferView];
            vbo.bind();

            int size = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                size = accessor.type;
            }

            int vaa = -1;
            if (attrib.first == "POSITION") vaa = 0;
            if (attrib.first == "NORMAL") vaa = 1;
            if (attrib.first == "TEXCOORD_0") vaa = 2;
            if (vaa > -1) {
                program_.bind();

                program_.enableAttributeArray(vaa);
                program_.setAttributeBuffer(vaa, accessor.componentType,
                                            static_cast<int>(accessor.byteOffset),
                                            size, byteStride);

                program_.release();
            } else {
                std::cout << "vaa missing: " << attrib.first << std::endl;
            }

            vbo.release();
        }

        if (!model_->textures.empty()) {
            // fixme: Use material's baseColor
            tinygltf::Texture &tex = model_->textures[0];

            if (tex.source > -1) {

                GLuint texid;
                funcs_.glGenTextures(1, &texid);

                tinygltf::Image &image = model_->images[tex.source];

                funcs_.glBindTexture(GL_TEXTURE_2D, texid);
                funcs_.glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                funcs_.glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                funcs_.glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                funcs_.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                funcs_.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLenum format = GL_RGBA;

                if (image.component == 1) {
                    format = GL_RED;
                } else if (image.component == 2) {
                    format = GL_RG;
                } else if (image.component == 3) {
                    format = GL_RGB;
                } else {
                    // ???
                }

                GLenum type = GL_UNSIGNED_BYTE;
                if (image.bits == 8) {
                    // ok
                } else if (image.bits == 16) {
                    type = GL_UNSIGNED_SHORT;
                } else {
                    // ???
                }

                funcs_.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                                    format, type, &image.image.at(0));
            }
        }
    }
}

void ModelHolder::drawModel() {
    vao_.bind();

    const tinygltf::Scene &scene = model_->scenes[model_->defaultScene];
    for (int node: scene.nodes)
        drawModelNodes(model_->nodes[node]);

    vao_.release();
}

void ModelHolder::drawModelNodes(tinygltf::Node &node) {
    if ((node.mesh >= 0) && (node.mesh < model_->meshes.size()))
        drawMesh(model_->meshes[node.mesh]);

    for (int i: node.children)
        drawModelNodes(model_->nodes[i]);
}

void ModelHolder::drawMesh(tinygltf::Mesh &mesh) {
    for (const auto& primitive : mesh.primitives) {
        tinygltf::Accessor indexAccessor = model_->accessors[primitive.indices];

        QOpenGLBuffer& vbo = vbos_.at(indexAccessor.bufferView);
        vbo.bind();

        funcs_.glDrawElements(primitive.mode, static_cast<GLsizei>(indexAccessor.count),
                              indexAccessor.componentType,
                              BUFFER_OFFSET(indexAccessor.byteOffset));

        vbo.release();
    }
}
