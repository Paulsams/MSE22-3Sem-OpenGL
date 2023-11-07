#include "LoaderModel.h"
#include "SceneNode.h"
#include "LoadModelIterator.h"

#include <iostream>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

std::shared_ptr<SceneNode> LoaderModel::load(const char *filename, QOpenGLFunctions &glFuncs,
                                             const std::shared_ptr<QOpenGLShaderProgram> &program) {

    auto model = loadModel(filename);
    auto newNode = LoadModelIterator(glFuncs, program, *model).create();
    return newNode;
}

std::unique_ptr<tinygltf::Model> LoaderModel::loadModel(const char *filename) {
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
