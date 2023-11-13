#pragma once
#include <QOpenGLExtraFunctions>

class OpenGLUniformBuffer {
public:
    explicit OpenGLUniformBuffer(QOpenGLExtraFunctions& funcs);

    void create();
    void allocate(int blockSize, int blockBinding);
    void bind();
    void release();

    void setSubData(int offset, int size, const void* memory);

private:
    GLuint id_;
    QOpenGLExtraFunctions& funcs_;
};
