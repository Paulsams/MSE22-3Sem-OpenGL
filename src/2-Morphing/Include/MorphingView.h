#pragma once

#include "Base/GLWidget.hpp"
#include "Base/DebugWindow.h"
#include "Base/FrameCounter.h"
#include "Base/Time.h"

#include "tiny_gltf.h"
#include "ModelHolder.h"
#include "CameraView.h"

#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <functional>
#include <memory>

class MorphingView final : public fgl::GLWidget
{
	Q_OBJECT
public:
	MorphingView() noexcept;
	~MorphingView() override;

public: // fgl::GLWidget
	void onInit() override;
	void onRender() override;
	void onResize(size_t width, size_t height) override;

private:
    DebugWindow *debugWindow_;
    FrameCounter frameCounter_;

	QMatrix4x4 model_;
	QMatrix4x4 view_;
	QMatrix4x4 projection_;

	std::unique_ptr<QOpenGLTexture> texture_ = nullptr;
	std::unique_ptr<QOpenGLShaderProgram> program_;
    ModelHolder modelHolder_;
    CameraView cameraView_;
    Time time_{};

	bool animated_ = true;
};
