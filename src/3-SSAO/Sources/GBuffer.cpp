#include "GBuffer.h"

namespace SSAOImpl {
    GBuffer::GBuffer(QOpenGLExtraFunctions& funcs)
            : funcs_(funcs) { }

    void GBuffer::resize(const size_t width, const size_t height, int) {
        QOpenGLFramebufferObjectFormat format;
//        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setAttachment(QOpenGLFramebufferObject::NoAttachment);
        format.setInternalTextureFormat(GL_RGBA32F);
//    format.setSamples(countSamples);

        int widthI = static_cast<int>(width);
        int heightI = static_cast<int>(height);

        fbo_ = std::make_unique<QOpenGLFramebufferObject>(widthI, heightI, format);
        for (int i = 1; i <= GBUFFER_TEXTURE_TYPE_SSAO; ++i)
            fbo_->addColorAttachment(widthI, heightI, GL_RGBA32F);
        for (int i = GBUFFER_TEXTURE_TYPE_SSAO + 1; i <= GBUFFER_NUM_TEXTURES; ++i)
            fbo_->addColorAttachment(widthI, heightI, GL_RGB);

        fbo_->bind();

        funcs_.glGenTextures(1, &depthAttachment_);
        funcs_.glBindTexture(GL_TEXTURE_2D, depthAttachment_);
        funcs_.glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, widthI, heightI, 0, GL_DEPTH_STENCIL,
                            GL_UNSIGNED_INT_24_8, nullptr);
        funcs_.glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        funcs_.glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        funcs_.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachment_, 0);

//        funcs_.glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
//                                                     GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &depthAttachment_);
        fbo_->release();

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
//        funcs_.glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        funcs_.glClear(GL_COLOR_BUFFER_BIT);
//        funcs_.glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }

    void GBuffer::bindForGeomPass() {
        funcs_.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_->handle());

        GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0,
                                 GL_COLOR_ATTACHMENT1,
                                 GL_COLOR_ATTACHMENT2,
        };

        funcs_.glDrawBuffers(sizeof(drawBuffers) / sizeof(GLenum), drawBuffers);
    }

    void GBuffer::bindForSSAOPass() {
        funcs_.glBindFramebuffer(GL_FRAMEBUFFER, fbo_->handle());

        GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_SSAO };
        funcs_.glDrawBuffers(1, drawBuffers);
        funcs_.glClear(GL_COLOR_BUFFER_BIT);
    }

    void GBuffer::bindForBlurPass() {
        funcs_.glBindFramebuffer(GL_FRAMEBUFFER, fbo_->handle());

        GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_SSAO_WITH_BLUR };
        funcs_.glDrawBuffers(1, drawBuffers);
        funcs_.glClear(GL_COLOR_BUFFER_BIT);

        bindTexture(GBUFFER_TEXTURE_TYPE_SSAO);
    }

    void GBuffer::bindForStencilPass() {
        GLenum drawBuffers[] = { GL_NONE };
        funcs_.glDrawBuffers(1, drawBuffers);
    }

    void GBuffer::bindForLightPass() {
        GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 + GBUFFER_NUM_TEXTURES };
        funcs_.glDrawBuffers(1, drawBuffers);

        bindTexture(GBUFFER_TEXTURE_TYPE_POSITION);
        bindTexture(GBUFFER_TEXTURE_TYPE_DIFFUSE);
        bindTexture(GBUFFER_TEXTURE_TYPE_NORMAL);
        bindTexture(GBUFFER_TEXTURE_TYPE_SSAO_WITH_BLUR);
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

    void GBuffer::bindTexture(int indexTextureType) {
        funcs_.glActiveTexture(GL_TEXTURE0 + indexTextureType);
        funcs_.glBindTexture(GL_TEXTURE_2D, fbo_->textures()[indexTextureType]);
    }
} // SSAOImpl
