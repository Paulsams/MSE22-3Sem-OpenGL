#include "GBuffer.h"

GBuffer::GBuffer(QOpenGLExtraFunctions& funcs)
	: funcs_(funcs) { }

void GBuffer::resize(const size_t width, const size_t height, int countSamples) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setInternalTextureFormat(GL_RGBA32F);
//    format.setSamples(countSamples);

    int witdhI = static_cast<int>(width);
    int heightI = static_cast<int>(height);

    fbo_ = std::make_unique<QOpenGLFramebufferObject>(witdhI, heightI, format);
    for (int i = 1; i < GBUFFER_NUM_TEXTURES; ++i)
        fbo_->addColorAttachment(witdhI, heightI, GL_RGBA32F);
    fbo_->addColorAttachment(witdhI, heightI, GL_RGB);

    auto textures = fbo_->textures();
    for (int i = 0; i < GBUFFER_NUM_TEXTURES; ++i) {
        funcs_.glBindTexture(GL_TEXTURE_2D, textures[i]);
        funcs_.glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        funcs_.glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
}

void GBuffer::startFrame() {
    funcs_.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_->handle());
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 + GBUFFER_NUM_TEXTURES };
    funcs_.glDrawBuffers(1, drawBuffers);
    funcs_.glClear(GL_COLOR_BUFFER_BIT);
}

void GBuffer::bindForGeomPass() {
    funcs_.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_->handle());

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0,
                             GL_COLOR_ATTACHMENT1,
                             GL_COLOR_ATTACHMENT2 };

    funcs_.glDrawBuffers(sizeof(drawBuffers) / sizeof(GLenum), drawBuffers);
}

void GBuffer::bindForStencilPass() {
    GLenum drawBuffers[] = { GL_NONE };
    funcs_.glDrawBuffers(1, drawBuffers);
}

void GBuffer::bindForLightPass() {
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 + GBUFFER_NUM_TEXTURES };
    funcs_.glDrawBuffers(1, drawBuffers);

    auto textures = fbo_->textures();
    for (int i = 0 ; i < GBUFFER_NUM_TEXTURES; ++i) {
        funcs_.glActiveTexture(GL_TEXTURE0 + i);
        funcs_.glBindTexture(GL_TEXTURE_2D, textures[i]);
    }
}

void GBuffer::blit(
        QOpenGLFramebufferObject* target, const QRect& targetRect,
        const QRect& sourceRect,
        int readColorAttachmentIndex, int drawColorAttachmentIndex,
        GLbitfield buffers, GLenum filter) {
    QOpenGLFramebufferObject::blitFramebuffer(
            target, targetRect,
            fbo_.get(), sourceRect,
            buffers, filter,
            readColorAttachmentIndex, drawColorAttachmentIndex
    );
}
