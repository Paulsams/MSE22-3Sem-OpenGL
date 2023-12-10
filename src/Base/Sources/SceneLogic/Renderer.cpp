#include <iostream>

#include "SceneLogic/Renderer.h"

Renderer::Renderer(QOpenGLFunctions &funcs)
        : funcs_(funcs) {}

void Renderer::draw(const QMatrix4x4& model, const QMatrix4x4 &viewProjection) {
    vao_.bind();
    indexBuffer_.bind();

    program_->setUniformValue(mvpUniformLocation_, viewProjection * model);
    program_->setUniformValue(modelUniformLocation_, model);
    program_->setUniformValue(transposeInverseModelUniformLocation_, model.inverted().transposed().toGenericMatrix<3, 3>());

    if (!textures_.empty()) {
        if (textures_[0].second.texture != nullptr) {
            funcs_.glActiveTexture(GL_TEXTURE5);
            textures_[0].second.texture->bind();
            program_->setUniformValue("duffuse_tex", 5);
        }
        program_->setUniformValue("duffuse_color", textures_[0].second.color);
    }

    funcs_.glDrawElements(indicesInfo_.primitiveMode, indicesInfo_.count,
                          indicesInfo_.componentType, indicesInfo_.indices);

    if (!textures_.empty() && textures_[0].second.texture != nullptr)
        textures_[0].second.texture->release();

    indexBuffer_.release();
    vao_.release();
}

void Renderer::init(const IndicesInfo &indicesInfo,
                    const BufferData &indexesData,
                    ProgramInfoFromObject &programInfoFromObject,
                    const TexturesContainer&& textures) {
    textures_ = std::move(textures);
    indicesInfo_ = indicesInfo;

    vao_.create();
    vao_.bind();

    for (const auto &[_, attributeInfo]: programInfoFromObject.attributes) {
        QOpenGLBuffer vertexBuffer{QOpenGLBuffer::Type::VertexBuffer};
        bindOpenGLBuffer(vertexBuffer, attributeInfo.bufferData);
        vertexBuffers_.push_back(vertexBuffer);
    }
    bindOpenGLBuffer(indexBuffer_, indexesData);

    // TODO: убрать зависимость от программы
    program_ = programInfoFromObject.program;
    program_->bind();

    mvpUniformLocation_ = program_->uniformLocation("mvp");
    modelUniformLocation_ = program_->uniformLocation("model");
    transposeInverseModelUniformLocation_ = program_->uniformLocation("transpose_inverse_model");

    for (size_t i = 0; i < programInfoFromObject.attributes.size(); ++i) {
        auto& buffer = vertexBuffers_[i];
        auto&[location, attributeInfo] = programInfoFromObject.attributes[i];

        buffer.bind();

        program_->enableAttributeArray(location);
        program_->setAttributeBuffer(location, attributeInfo.componentType,
                                     attributeInfo.offset,
                                     attributeInfo.size, attributeInfo.stride);
        buffer.release();
    }

    program_->release();
    vao_.release();
}

void Renderer::bindOpenGLBuffer(QOpenGLBuffer &buffer, const BufferData &bufferData) {
    buffer.create();
    buffer.bind();
    buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

    buffer.allocate(bufferData.data, bufferData.count);

    buffer.release();
}
