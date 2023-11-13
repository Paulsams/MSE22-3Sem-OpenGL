#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include "tiny_gltf.h"

class SceneNode;

class LoaderModel {
public:
    static std::shared_ptr<SceneNode> load(const char *filename, QOpenGLFunctions &glFuncs,
                                           const std::shared_ptr<QOpenGLShaderProgram> &program);

private:
    static std::unique_ptr<tinygltf::Model> loadModel(const char *filename);
};
