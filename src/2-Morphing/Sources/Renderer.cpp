#include "Renderer.h"

Renderer::Renderer(QOpenGLFunctions &funcs)
        : funcs_(funcs) {}

void Renderer::draw(const QMatrix4x4 &viewProjection) {
    vao_.bind();

    program_->setUniformValue("mvp", viewProjection * model_);
    program_->setUniformValue("model", model_);

    funcs_.glDrawElements(indicesInfo_.primitiveMode, indicesInfo_.count,
                          indicesInfo_.componentType, indicesInfo_.indices);

    vao_.release();
}

void Renderer::init(const IndicesInfo &indicesInfo, const std::vector<BufferData> &verticesData, const BufferData &indexesData,
                    ProgramInfoFromObject &programInfoFromObject) {
    indicesInfo_ = indicesInfo;

    vao_.create();
    vao_.bind();

    for (const auto &item: verticesData)
        bindOpenGLBuffer(vertexBuffer_, item);
    bindOpenGLBuffer(indexBuffer_, indexesData);

    program_ = programInfoFromObject.program;
    program_->bind();

    for (const auto &[location, attributeInfo]: programInfoFromObject.attributes) {
        program_->enableAttributeArray(location);
        program_->setAttributeBuffer(0, attributeInfo.componentType,
                                     attributeInfo.offset,
                                     attributeInfo.size, attributeInfo.stride);
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
