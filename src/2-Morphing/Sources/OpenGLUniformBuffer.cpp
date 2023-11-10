#include "OpenGLUniformBuffer.h"

OpenGLUniformBuffer::OpenGLUniformBuffer(QOpenGLExtraFunctions&funcs)
    : funcs_(funcs) {
}

void OpenGLUniformBuffer::create() {
    funcs_.glGenBuffers(1, &id_);
}

void OpenGLUniformBuffer::allocate(const int blockSize) {
    funcs_.glBufferData(GL_UNIFORM_BUFFER, blockSize, nullptr, GL_STATIC_DRAW);
    funcs_.glBindBufferRange(GL_UNIFORM_BUFFER, 0, id_, 0, blockSize);
}

void OpenGLUniformBuffer::bind() {
    funcs_.glBindBuffer(GL_UNIFORM_BUFFER, id_);
}

void OpenGLUniformBuffer::release() {
    funcs_.glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLUniformBuffer::setSubData(const int offset, const int size, const void* memory) {
    funcs_.glBufferSubData(GL_UNIFORM_BUFFER, offset, size, memory);
}
