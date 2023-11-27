#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

struct BufferData {
    const void* data;
    int count;

    BufferData() = delete;

    BufferData(const void* data, int count)
        : data(data), count(count) {
    }
};

struct AttributeInfo {
    GLenum componentType;
    int offset;
    int size;
    int stride;
    BufferData bufferData;

    AttributeInfo() = delete;

    AttributeInfo(GLenum componentType, int offset, int size, int stride, BufferData bufferData)
        : componentType(componentType), offset(offset), size(size), stride(stride), bufferData(bufferData) {
    }
};

struct ProgramInfoFromObject {
    std::shared_ptr<QOpenGLShaderProgram> program;
    std::vector<std::pair<GLint, AttributeInfo>> attributes;

    ProgramInfoFromObject() = delete;

    ProgramInfoFromObject(const std::shared_ptr<QOpenGLShaderProgram>& program,
                          const std::vector<std::pair<GLint, AttributeInfo>>&& attributes)
        : program(program), attributes(std::move(attributes)) {
    }
};

struct IndicesInfo {
    GLenum primitiveMode;
    GLsizei count;
    GLenum componentType;
    const GLvoid* indices;

    IndicesInfo() = delete;

    IndicesInfo(GLenum primitiveMode, GLsizei count, GLenum componentType, const void* indices)
        : primitiveMode(primitiveMode), count(count), componentType(componentType), indices(indices) {
    }
};

enum struct TextureType {
    Diffuse,
};

struct TextureMapperInfo {
    QVector3D color;
    // TODO: Мейби тут??? QOpenGLTexturePrivate::destroy() called without a current context. Texture has not been destroyed.
    std::shared_ptr<QOpenGLTexture> texture;

    TextureMapperInfo(const QVector3D& color, const std::shared_ptr<QOpenGLTexture>& texture)
        : color(color)
        , texture(texture) {
    }
};

class Renderer {
public:
    using TexturesContainer = std::vector<std::pair<TextureType, TextureMapperInfo>>;

    void init(const IndicesInfo& indicesInfo,
              const BufferData& indexesData,
              ProgramInfoFromObject& programInfoFromObject,
              const TexturesContainer&& textures);

    void draw(const QMatrix4x4& model, const QMatrix4x4& viewProjection);

    explicit Renderer(QOpenGLFunctions&funcs);

private:
    static void bindOpenGLBuffer(QOpenGLBuffer&buffer, const BufferData&bufferData);

    int mvpUniformLocation_;
    int modelUniformLocation_;
    int transposeInverseModelUniformLocation_;

    TexturesContainer textures_;
    IndicesInfo indicesInfo_{0, 0, 0, nullptr};
    QOpenGLFunctions&funcs_;
    QOpenGLVertexArrayObject vao_;
    std::shared_ptr<QOpenGLShaderProgram> program_{};
    std::vector<QOpenGLBuffer> vertexBuffers_;
    QOpenGLBuffer indexBuffer_{QOpenGLBuffer::Type::IndexBuffer};
};
