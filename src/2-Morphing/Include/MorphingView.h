#pragma once

#include "Base/GLWidget.hpp"
#include "Base/DebugWindow.h"
#include "Base/FrameCounter.h"
#include "Base/Time.h"

#include "tiny_gltf.h"
#include "CameraView.h"

#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

#include <memory>
#include <qopengltexture.h>

#include "OpenGLUniformBuffer.h"

class Scene;
class SceneNode;

class LightInfo {
    QColor color;
};

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
    std::unique_ptr<Scene> scene_;
    std::shared_ptr<SceneNode> nodeDirectionalLight_;

    DebugWindow *debugWindow_;
    FrameCounter frameCounter_;

	std::unique_ptr<QOpenGLTexture> texture_ = nullptr;
	std::shared_ptr<QOpenGLShaderProgram> program_;

	OpenGLUniformBuffer pointLights;
	OpenGLUniformBuffer spotLights;
	OpenGLUniformBuffer directionalLight;

    CameraView cameraView_;
    Time time_{};

	bool animated_ = true;

    float ambientStrength = 0.1f;
    float specularStrength = 0.5f;
    int powerSpecular = 128;
    QColor lightColor = QColor(255, 255, 255);
};
