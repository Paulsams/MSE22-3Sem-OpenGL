#include "GBuffer.h"

GBuffer::GBuffer(QOpenGLExtraFunctions& funcs)
	: funcs_(funcs) { }

void GBuffer::resize(const size_t width, const size_t height, int countSamples)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(countSamples);

	int witdhI = static_cast<int>(width);
	int heightI = static_cast<int>(height);

	fbo_ = std::make_unique<QOpenGLFramebufferObject>(witdhI, heightI, format);
	fbo_->addColorAttachment(witdhI, heightI);
	fbo_->addColorAttachment(witdhI, heightI);
	fbo_->addColorAttachment(witdhI, heightI);

    fbo_->bind();
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    funcs_.glDrawBuffers(sizeof(drawBuffers) / sizeof(GLenum), drawBuffers);
    fbo_->release();
}

void GBuffer::bindForWriting()
{
	funcs_.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_->handle());
}

void GBuffer::bindForReading()
{
	funcs_.glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_->handle());
}

void GBuffer::release()
{
	fbo_->release();
}

void GBuffer::setReadBuffer(GBUFFER_TEXTURE_TYPE textureType)
{
	funcs_.glReadBuffer(GL_COLOR_ATTACHMENT0 + textureType);
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
