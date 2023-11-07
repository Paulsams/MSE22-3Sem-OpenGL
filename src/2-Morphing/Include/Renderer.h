#pragma once

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

struct BufferData {
    const void *data;
    int count;

    BufferData() = delete;

    BufferData(const void *data, int count)
            : data(data), count(count) {}
};

struct AttributeInfo {
    GLenum componentType;
    int offset;
    int size;
    int stride;

    AttributeInfo() = delete;

    AttributeInfo(GLenum componentType, int offset, int size, int stride)
            : componentType(componentType), offset(offset), size(size), stride(stride) {}
};

struct ProgramInfoFromObject {
    std::shared_ptr<QOpenGLShaderProgram> program;
    std::vector<std::pair<GLint, AttributeInfo>> attributes;

    ProgramInfoFromObject() = delete;

    ProgramInfoFromObject(const std::shared_ptr<QOpenGLShaderProgram> &program,
                          const std::vector<std::pair<GLint, AttributeInfo>> &&attributes)
            : program(program), attributes(attributes) {}
};

struct IndicesInfo {
    GLenum primitiveMode;
    GLsizei count;
    GLenum componentType;
    const GLvoid *indices;

    IndicesInfo() = delete;

    IndicesInfo(GLenum primitiveMode, GLsizei count, GLenum componentType, const void *indices)
            : primitiveMode(primitiveMode), count(count), componentType(componentType), indices(indices) {}
};

class Renderer {
public:

    void
    init(const IndicesInfo &indicesInfo, const std::vector<BufferData> &verticesData, const BufferData &indexesData,
         ProgramInfoFromObject &programInfoFromObject);

    void draw(const QMatrix4x4 &viewProjection);

    explicit Renderer(QOpenGLFunctions &funcs);

private:
    static void bindOpenGLBuffer(QOpenGLBuffer &buffer, const BufferData &bufferData);

    QMatrix4x4 model_;

    IndicesInfo indicesInfo_{0, 0, 0, nullptr};
    QOpenGLFunctions &funcs_;
    QOpenGLVertexArrayObject vao_;
    std::shared_ptr<QOpenGLShaderProgram> program_{};
    QOpenGLBuffer vertexBuffer_{QOpenGLBuffer::Type::VertexBuffer};
    QOpenGLBuffer indexBuffer_{QOpenGLBuffer::Type::IndexBuffer};
};
