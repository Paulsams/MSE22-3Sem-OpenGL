#pragma once

#include <QOpenGLExtraFunctions>

class OpenGLUniformBuffer {
public:
    explicit OpenGLUniformBuffer(QOpenGLExtraFunctions& funcs);

    void create();
    void allocate(int blockSize, int blockBindingIndex);
    void bind();
    void release();

    void bindForProgram();

    void setSubData(int offset, int size, const void* memory);

private:
    GLuint id_;
    QOpenGLExtraFunctions& funcs_;

    int blockSize_;
    int blockBindingIndex_;
};
