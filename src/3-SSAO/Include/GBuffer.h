#pragma once

#include "Deferred/IGBuffer.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <memory>

namespace SSAOImpl {
    class GBuffer : public IGBuffer {
    public:
        enum GBUFFER_TEXTURE_TYPE {
            GBUFFER_TEXTURE_TYPE_POSITION,
            GBUFFER_TEXTURE_TYPE_DIFFUSE,
            GBUFFER_TEXTURE_TYPE_NORMAL,
            GBUFFER_TEXTURE_TYPE_POSITION_VIEW_SPACE,
            GBUFFER_TEXTURE_TYPE_SSAO,
            GBUFFER_TEXTURE_TYPE_SSAO_WITH_BLUR,
            GBUFFER_NUM_TEXTURES
        };

        explicit GBuffer(QOpenGLExtraFunctions& funcs);

        QOpenGLFramebufferObject& getFBO() { return *fbo_; }

        void resize(size_t width, size_t height, int countSamples = 0);

        void startFrame();
        void bindForGeomPass();
        void bindForBlurPass();
        void bindForSSAOPass();
        void bindForStencilPass() override;
        void bindForLightPass() override;

        void bindTexture(int indexTextureType);

        void blit(
            QOpenGLFramebufferObject * target, const QRect & targetRect, const QRect & sourceRect,
            int readColorAttachmentIndex,
            int drawColorAttachmentIndex = 0,
            GLbitfield buffers = GL_COLOR_BUFFER_BIT,
            GLenum filter = GL_NEAREST);

    private:
        std::unique_ptr<QOpenGLFramebufferObject> fbo_ = nullptr;
        QOpenGLExtraFunctions& funcs_;
    };
} // SSAOImpl

