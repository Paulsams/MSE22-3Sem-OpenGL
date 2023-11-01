#pragma once

#include <memory>
#include <QtGui>
#include "tiny_gltf.h"

class ModelHolder {
public:
    explicit ModelHolder(const char *filename, QOpenGLFunctions &glFuncs, QOpenGLShaderProgram &program);

    void init();

    void draw();

private:
    static std::unique_ptr<tinygltf::Model> loadModel(const char *filename);

    void bindMesh(tinygltf::Mesh &mesh);

    void bindModelNodes(tinygltf::Node &node);

    void bindModel();

    void drawModelNodes(tinygltf::Node &node);

    void drawModel();

    void drawMesh(tinygltf::Mesh &mesh);

//    QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
//    QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
    QOpenGLVertexArrayObject vao_;

    std::map<size_t, QOpenGLBuffer> vbos_;

    std::unique_ptr<tinygltf::Model> model_;
    QOpenGLFunctions &funcs_;
    QOpenGLShaderProgram &program_;
};
