#include <iostream>

#include "SceneLogic/Renderer.h"

Renderer::Renderer(QOpenGLFunctions &funcs)
        : funcs_(funcs) {}

void Renderer::draw(const QMatrix4x4& model, const QMatrix4x4& view, const QMatrix4x4& viewProjection) {
    vao_.bind();
    if (indicesContainer_.has_value())
        indicesContainer_->buffer.bind();

    program_->setUniformValue(mvpUniformLocation_, viewProjection * model);
    program_->setUniformValue(modelUniformLocation_, model);
    program_->setUniformValue("mv", view * model);
    program_->setUniformValue(transposeInverseModelUniformLocation_, model.inverted().transposed().toGenericMatrix<3, 3>());

    if (!textures_.empty()) {
        if (textures_[0].second.texture != nullptr) {
            funcs_.glActiveTexture(GL_TEXTURE8);
            textures_[0].second.texture->bind();
            program_->setUniformValue("duffuse_tex", 8);
        }
        program_->setUniformValue("duffuse_color", textures_[0].second.color);
    }

    if (indicesContainer_.has_value())
        funcs_.glDrawElements(indicesContainer_->info.primitiveMode, indicesContainer_->info.count,
                              indicesContainer_->info.componentType, indicesContainer_->info.bufferOffset);
    else
        funcs_.glDrawArrays(primitiveMode_, 0, countVertices_);

    if (!textures_.empty() && textures_[0].second.texture != nullptr)
        textures_[0].second.texture->release();

    if (indicesContainer_.has_value())
        indicesContainer_->buffer.release();
    vao_.release();
}

void Renderer::init(GLenum primitiveMode,
                    int countVertices,
                    ProgramInfoFromObject& programInfoFromObject,
                    std::optional<IndicesBuffer> indicesBuffer,
                    const TexturesContainer&& textures) {
    primitiveMode_ = primitiveMode;
    countVertices_ = countVertices;

    textures_ = std::move(textures);
    if (indicesBuffer.has_value())
        indicesContainer_ = { std::move(indicesBuffer->info) };

    vao_.create();
    vao_.bind();

    for (const auto &[_, attributeInfo]: programInfoFromObject.attributes) {
        QOpenGLBuffer vertexBuffer{QOpenGLBuffer::Type::VertexBuffer};
        bindOpenGLBuffer(vertexBuffer, attributeInfo.bufferData);
        vertexBuffers_.push_back(vertexBuffer);
    }
    if (indicesBuffer.has_value())
        bindOpenGLBuffer(indicesContainer_->buffer, indicesBuffer->buffer);

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
