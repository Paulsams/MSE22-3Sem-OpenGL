//#include <iostream>
//#include "ModelHolder.h"
//
//#define BUFFER_OFFSET(i) ((char *)NULL + (i))
//
//ModelHolder::ModelHolder(const char *filename, QOpenGLFunctions &glFuncs, QOpenGLShaderProgram &program)
//        : model_(loadModel(filename)), funcs_(glFuncs), program_(program) {}
//
//void ModelHolder::init() {
//    bindModel();
//}
//
//void ModelHolder::draw() {
//    drawModel();
//}
//
//std::unique_ptr<tinygltf::Model> ModelHolder::loadModel(const char *filename) {
//    tinygltf::Model model;
//    tinygltf::TinyGLTF loader;
//    std::string err;
//    std::string warn;
//
//    bool res = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
//    if (!warn.empty())
//        std::cout << "WARN: " << warn << std::endl;
//
//    if (!err.empty())
//        std::cout << "ERR: " << err << std::endl;
//
//    if (!res)
//        std::cout << "Failed to load glTF: " << filename << std::endl;
//    else
//        std::cout << "Loaded glTF: " << filename << std::endl;
//
//    return std::make_unique<tinygltf::Model>(model);
//}
//
//void ModelHolder::bindModel() {
//    vao_.create();
//    vao_.bind();
//
//    const tinygltf::Scene &scene = model_->scenes[model_->defaultScene];
//    for (int node: scene.nodes) {
//        assert((node >= 0) && (node < model_->nodes.size()));
//        initBufferForModelNodes(model_->nodes[node]);
//    }
//
////    glBindVertexArray(0);
////    // cleanup vbos but do not delete index buffers yet
////    for (auto it = vbos.cbegin(); it != vbos.cend();) {
////        tinygltf::BufferView bufferView = model.bufferViews[it->first];
////        if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
////            glDeleteBuffers(1, &vbos[it->first]);
////            vbos.erase(it++);
////        }
////        else {
////            ++it;
////        }
////    }
//
//    vao_.release();
//}
//
//void ModelHolder::initBufferForModelNodes(tinygltf::Node &node) {
//    if ((node.mesh >= 0) && (node.mesh < model_->meshes.size())) {
//        initBufferForMesh(model_->meshes[node.mesh]);
//    }
//
//    for (int i: node.children) {
//        assert((i >= 0) && (i < model_->nodes.size()));
//        initBufferForModelNodes(model_->nodes[i]);
//    }
//}
//
//void ModelHolder::initBufferForMesh(tinygltf::Mesh &mesh) {
//    {
//        for (size_t i = 0; i < model_->bufferViews.size(); i++) {
//            const tinygltf::BufferView &bufferView = model_->bufferViews[i];
//            if (bufferView.target == 0) {
//                std::cout << "WARN: bufferView.target is zero" << std::endl;
//                continue;  // Unsupported bufferView.
//            }
//
//            int sparse_accessor = -1;
//            for (size_t a_i = 0; a_i < model_->accessors.size(); ++a_i) {
//                const auto &accessor = model_->accessors[a_i];
//                if (accessor.bufferView == i) {
//                    std::cout << i << " is used by accessor " << a_i << std::endl;
//                    if (accessor.sparse.isSparse) {
//                        std::cout
//                                << "WARN: this bufferView has at least one sparse accessor to "
//                                   "it. We are going to load the data as patched by this "
//                                   "sparse accessor, not the original data"
//                                << std::endl;
//                        sparse_accessor = a_i;
//                        break;
//                    }
//                }
//            }
//
//            const tinygltf::Buffer &buffer = model_->buffers[bufferView.buffer];
//            QOpenGLBuffer vbo{bufferView.target == GL_ELEMENT_ARRAY_BUFFER ? QOpenGLBuffer::Type::IndexBuffer
//                                                                           : QOpenGLBuffer::Type::VertexBuffer};
//            vbo.create();
//            vbo.bind();
//            vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
//            std::cout << "buffer.size= " << buffer.data.size()
//                      << ", byteOffset = " << bufferView.byteOffset << std::endl;
//
//            if (sparse_accessor < 0) {
//                vbo.allocate(&buffer.data.at(0) + bufferView.byteOffset,
//                             static_cast<int>(bufferView.byteLength));
////                glBufferData(bufferView.target, bufferView.byteLength,
////                             &buffer.data.at(0) + bufferView.byteOffset,
////                             GL_STATIC_DRAW);
//            } else {
//                const auto accessor = model_->accessors[sparse_accessor];
//                // copy the buffer to a temporary one for sparse patching
//                unsigned char *tmp_buffer = new unsigned char[bufferView.byteLength];
//                memcpy(tmp_buffer, buffer.data.data() + bufferView.byteOffset,
//                       bufferView.byteLength);
//
//                const size_t size_of_object_in_buffer =
//                        componentTypeByteSize(accessor.componentType);
//                const size_t size_of_sparse_indices =
//                        componentTypeByteSize(accessor.sparse.indices.componentType);
//
//                const auto &indices_buffer_view =
//                        model_->bufferViews[accessor.sparse.indices.bufferView];
//                const auto &indices_buffer = model_->buffers[indices_buffer_view.buffer];
//
//                const auto &values_buffer_view =
//                        model_->bufferViews[accessor.sparse.values.bufferView];
//                const auto &values_buffer = model_->buffers[values_buffer_view.buffer];
//
//                for (size_t sparse_index = 0; sparse_index < accessor.sparse.count;
//                     ++sparse_index) {
//                    int index = 0;
//                    // std::cout << "accessor.sparse.indices.componentType = " <<
//                    // accessor.sparse.indices.componentType << std::endl;
//                    switch (accessor.sparse.indices.componentType) {
//                        case TINYGLTF_COMPONENT_TYPE_BYTE:
//                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
//                            index = (int) *(
//                                    unsigned char *) (indices_buffer.data.data() +
//                                                      indices_buffer_view.byteOffset +
//                                                      accessor.sparse.indices.byteOffset +
//                                                      (sparse_index * size_of_sparse_indices));
//                            break;
//                        case TINYGLTF_COMPONENT_TYPE_SHORT:
//                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
//                            index = (int) *(
//                                    unsigned short *) (indices_buffer.data.data() +
//                                                       indices_buffer_view.byteOffset +
//                                                       accessor.sparse.indices.byteOffset +
//                                                       (sparse_index * size_of_sparse_indices));
//                            break;
//                        case TINYGLTF_COMPONENT_TYPE_INT:
//                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
//                            index = (int) *(
//                                    unsigned int *) (indices_buffer.data.data() +
//                                                     indices_buffer_view.byteOffset +
//                                                     accessor.sparse.indices.byteOffset +
//                                                     (sparse_index * size_of_sparse_indices));
//                            break;
//                    }
//                    std::cout << "updating sparse data at index  : " << index
//                              << std::endl;
//                    // index is now the target of the sparse index to patch in
//                    const unsigned char *read_from =
//                            values_buffer.data.data() +
//                            (values_buffer_view.byteOffset +
//                             accessor.sparse.values.byteOffset) +
//                            (sparse_index * (size_of_object_in_buffer * accessor.type));
//
//                    /*
//                    std::cout << ((float*)read_from)[0] << "\n";
//                    std::cout << ((float*)read_from)[1] << "\n";
//                    std::cout << ((float*)read_from)[2] << "\n";
//                    */
//
//                    unsigned char *write_to =
//                            tmp_buffer + index * (size_of_object_in_buffer * accessor.type);
//
//                    memcpy(write_to, read_from, size_of_object_in_buffer * accessor.type);
//                }
//
//                // debug:
//                /*for(size_t p = 0; p < bufferView.byteLength/sizeof(float); p++)
//                {
//                  float* b = (float*)tmp_buffer;
//                  std::cout << "modified_buffer [" << p << "] = " << b[p] << '\n';
//                }*/
//
//                vbo.allocate(&tmp_buffer,
//                             static_cast<int>(bufferView.byteLength));
////                glBufferData(bufferView.target, bufferView.byteLength, tmp_buffer,
////                             GL_STATIC_DRAW);
//                delete[] tmp_buffer;
//            }
//            vbo.release();
//            bos_[i] = vbo;
//        }
//    }
//}
//
//void ModelHolder::drawModel() {
//    vao_.bind();
//
//    assert(!model_->scenes.empty());
//    int scene_to_display = model_->defaultScene > -1 ? model_->defaultScene : 0;
//    const tinygltf::Scene &scene = model_->scenes[scene_to_display];
//    for (int node: scene.nodes) {
//        drawModelNodes(model_->nodes[node]);
//    }
//
//    vao_.release();
//}
//
//void ModelHolder::drawModelNodes(tinygltf::Node &node) {
//    if ((node.mesh >= 0) && (node.mesh < model_->meshes.size())) {
//        assert(node.mesh < model_->meshes.size());
//        drawMesh(model_->meshes[node.mesh]);
//    }
//
//    for (int i: node.children) {
//        assert(node.children[i] < model_->nodes.size());
//        drawModelNodes(model_->nodes[i]);
//    }
//}
//
//void ModelHolder::drawMesh(tinygltf::Mesh &mesh) {
//    for (const auto &primitive: mesh.primitives) {
//        if (primitive.indices < 0) return;
//
////        tinygltf::Accessor indexAccessor = model_->accessors[primitive.indices];
////
////        QOpenGLBuffer& ibo = bos_.at(indexAccessor.bufferView);
////        ibo.bind();
////
////        funcs_.glDrawElements(primitive.mode, static_cast<GLsizei>(indexAccessor.count),
////                              indexAccessor.componentType,
////                              BUFFER_OFFSET(indexAccessor.byteOffset));
////
////        ibo.release();
//
//        auto attrib(primitive.attributes.begin());
//        auto itEnd(
//                primitive.attributes.end());
//
//        program_.bind();
//
//        for (; attrib != itEnd; attrib++) {
//            assert(attrib->second >= 0);
//            const tinygltf::Accessor &accessor = model_->accessors[attrib->second];
//            QOpenGLBuffer &vbo = bos_.at(accessor.bufferView);
//            vbo.bind();
//
//            int size = 1;
//            if (accessor.type == TINYGLTF_TYPE_SCALAR) {
//                size = 1;
//            } else if (accessor.type == TINYGLTF_TYPE_VEC2) {
//                size = 2;
//            } else if (accessor.type == TINYGLTF_TYPE_VEC3) {
//                size = 3;
//            } else if (accessor.type == TINYGLTF_TYPE_VEC4) {
//                size = 4;
//            } else {
//                assert(0);
//            }
//
//            int vaa = -1;
//            if (attrib->first.compare("POSITION") == 0) vaa = 0;
//            if (attrib->first.compare("NORMAL") == 0) vaa = 1;
//            if (attrib->first.compare("TEXCOORD_0") == 0) vaa = 2;
//            if (vaa > -1) {
//                int byteStride =
//                        accessor.ByteStride(model_->bufferViews[accessor.bufferView]);
//                assert(byteStride != -1);
//
//                program_.enableAttributeArray(vaa);
//                program_.setAttributeBuffer(vaa, accessor.componentType,
//                                            static_cast<int>(accessor.byteOffset),
//                                            size, byteStride);
//            }
//
//            vbo.release();
//        }
//
//        const tinygltf::Accessor &indexAccessor =
//                model_->accessors[primitive.indices];
//        QOpenGLBuffer &ibo = bos_.at(indexAccessor.bufferView);
//        ibo.bind();
//
//        int mode = -1;
//        if (primitive.mode == TINYGLTF_MODE_TRIANGLES) {
//            mode = GL_TRIANGLES;
//        } else if (primitive.mode == TINYGLTF_MODE_TRIANGLE_STRIP) {
//            mode = GL_TRIANGLE_STRIP;
//        } else if (primitive.mode == TINYGLTF_MODE_TRIANGLE_FAN) {
//            mode = GL_TRIANGLE_FAN;
//        } else if (primitive.mode == TINYGLTF_MODE_POINTS) {
//            mode = GL_POINTS;
//        } else if (primitive.mode == TINYGLTF_MODE_LINE) {
//            mode = GL_LINES;
//        } else if (primitive.mode == TINYGLTF_MODE_LINE_LOOP) {
//            mode = GL_LINE_LOOP;
//        } else {
//            assert(0);
//        }
//        funcs_.glDrawElements(mode, indexAccessor.count, indexAccessor.componentType,
//                              BUFFER_OFFSET(indexAccessor.byteOffset));
//
//        {
//            std::map<std::string, int>::const_iterator it(
//                    primitive.attributes.begin());
//            std::map<std::string, int>::const_iterator itEnd(
//                    primitive.attributes.end());
//
//            program_.enableAttributeArray(0);
//            program_.setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(2 * sizeof(GLfloat)));
//
//            for (; it != itEnd; it++) {
//                int vaa = -1;
//                if (attrib->first.compare("POSITION") == 0) vaa = 0;
//                if (attrib->first.compare("NORMAL") == 0) vaa = 1;
//                if (attrib->first.compare("TEXCOORD_0") == 0) vaa = 2;
//                if (vaa > -1) {
//                    program_.disableAttributeArray(vaa);
//                }
//            }
//        }
//
//        program_.release();
//    }
//}
//
//size_t ModelHolder::componentTypeByteSize(int type) {
//    switch (type) {
//        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
//        case TINYGLTF_COMPONENT_TYPE_BYTE:
//            return sizeof(char);
//        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
//        case TINYGLTF_COMPONENT_TYPE_SHORT:
//            return sizeof(short);
//        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
//        case TINYGLTF_COMPONENT_TYPE_INT:
//            return sizeof(int);
//        case TINYGLTF_COMPONENT_TYPE_FLOAT:
//            return sizeof(float);
//        case TINYGLTF_COMPONENT_TYPE_DOUBLE:
//            return sizeof(double);
//        default:
//            return 0;
//    }
//}
