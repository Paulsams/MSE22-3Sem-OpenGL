#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <optional>

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
    const GLvoid* bufferOffset;

    IndicesInfo() = delete;

    IndicesInfo(GLenum primitiveMode, GLsizei count, GLenum componentType, const void* bufferOffset)
        : primitiveMode(primitiveMode), count(count), componentType(componentType), bufferOffset(bufferOffset) {
    }
};

struct IndicesBuffer {
    IndicesInfo info;
    BufferData buffer;

    IndicesBuffer() = delete;

    IndicesBuffer(IndicesInfo&& info, BufferData&& buffer)
        : info(std::move(info)), buffer(std::move(buffer)) {
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

    void init(GLenum primitiveMode,
              int countVertices,
              ProgramInfoFromObject& programInfoFromObject,
              std::optional<IndicesBuffer> indicesBuffer,
              const TexturesContainer&& textures);

    void draw(const QMatrix4x4& model, const QMatrix4x4& view, const QMatrix4x4& viewProjection);

    explicit Renderer(QOpenGLFunctions&funcs);

private:
    struct IndexBufferContainer {
        IndicesInfo info;
        QOpenGLBuffer buffer{QOpenGLBuffer::Type::IndexBuffer};
    };

    static void bindOpenGLBuffer(QOpenGLBuffer&buffer, const BufferData&bufferData);

    int mvpUniformLocation_;
    int modelUniformLocation_;
    int transposeInverseModelUniformLocation_;

    GLenum primitiveMode_;

    TexturesContainer textures_;
    std::optional<IndexBufferContainer> indicesContainer_;
    QOpenGLFunctions&funcs_;
    QOpenGLVertexArrayObject vao_;
    std::shared_ptr<QOpenGLShaderProgram> program_{};

    int countVertices_;
    std::vector<QOpenGLBuffer> vertexBuffers_;
};
