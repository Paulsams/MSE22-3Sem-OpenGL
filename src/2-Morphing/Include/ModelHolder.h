#pragma once

#include <memory>
#include <QtGui>
#include "tiny_gltf.h"

class ModelHolder {
public:
    explicit ModelHolder(const char *filename, QOpenGLFunctions &glFuncs, QOpenGLShaderProgram &program);

    void init(GLint mvpUniform);

    void draw(QMatrix4x4& mvp);

private:
    static std::unique_ptr<tinygltf::Model> loadModel(const char *filename);

    void bindMesh(tinygltf::Mesh &mesh);

    void bindModelNodes(tinygltf::Node &node);

    void bindModel();

    void drawModelNodes(tinygltf::Node &node, QMatrix4x4 mvp);

    void drawModel(QMatrix4x4& mvp);

    void drawMesh(tinygltf::Mesh &mesh, QMatrix4x4 &mvp);

    QOpenGLVertexArrayObject vao_;

    std::map<size_t, QOpenGLBuffer> bos_;

    std::unique_ptr<tinygltf::Model> model_;
    QOpenGLFunctions &funcs_;
    QOpenGLShaderProgram &program_;
    GLint mvpUniform_ = -1;
};
