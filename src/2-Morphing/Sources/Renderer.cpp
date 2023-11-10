#include <iostream>
#include "Renderer.h"

Renderer::Renderer(QOpenGLFunctions &funcs)
        : funcs_(funcs) {}

void Renderer::draw(const QMatrix4x4& model, const QMatrix4x4 &viewProjection) {
    vao_.bind();
    indexBuffer_.bind();

    program_->setUniformValue("mvp", viewProjection * model);
    program_->setUniformValue("model", model);

    funcs_.glDrawElements(indicesInfo_.primitiveMode, indicesInfo_.count,
                          indicesInfo_.componentType, indicesInfo_.indices);

    indexBuffer_.release();
    vao_.release();
}

void Renderer::init(const IndicesInfo &indicesInfo,
                    const BufferData &indexesData,
                    ProgramInfoFromObject &programInfoFromObject) {
    indicesInfo_ = indicesInfo;

    vao_.create();
    vao_.bind();

    for (const auto &[_, attributeInfo]: programInfoFromObject.attributes) {
        QOpenGLBuffer vertexBuffer{QOpenGLBuffer::Type::VertexBuffer};
        bindOpenGLBuffer(vertexBuffer, attributeInfo.bufferData);
        vertexBuffers_.push_back(vertexBuffer);
    }
    bindOpenGLBuffer(indexBuffer_, indexesData);

    program_ = programInfoFromObject.program;
    program_->bind();

    for (int i = 0; i < programInfoFromObject.attributes.size(); ++i) {
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
