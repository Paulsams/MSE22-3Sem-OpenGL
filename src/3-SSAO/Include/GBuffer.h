#pragma once

#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <memory>

class GBuffer {
public:
	enum GBUFFER_TEXTURE_TYPE {
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_TEXCOORD,
		GBUFFER_NUM_TEXTURES
	};

	explicit GBuffer(QOpenGLExtraFunctions& funcs);

	QOpenGLFramebufferObject& getFBO() { return *fbo_; }

	void resize(size_t width, size_t height, int countSamples = 0);

	void bindForWriting();

	void bindForReading();

    void setReadBuffer(GBUFFER_TEXTURE_TYPE textureType);

	void release();

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
