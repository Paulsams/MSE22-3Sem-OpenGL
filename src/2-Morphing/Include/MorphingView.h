#pragma once

#include "Base/Include/GLWidget.hpp"
#include "Base/Include/Utils/FrameCounter.h"
#include "Base/Include/Utils/Time.h"
#include "Base/Include/SceneLogic/Camera/CameraView.h"
#include "Base/Include/Views/InspectorView.h"
#include "Base/Include/Views/SceneHierarchyView.h"

#include <memory>

#include <QElapsedTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

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

    FieldsDrawer *debugWindow_;
    InspectorView *inspector_;
    SceneHierarchyView *sceneView_;
    FrameCounter frameCounter_;

	std::shared_ptr<QOpenGLShaderProgram> program_;

    CameraView cameraView_;
    Time time_{};

	bool animated_ = true;

    bool isMorphing_ = false;
    int powerSpecular_ = 128;
    QColor clearColor_ = QColor(70, 70, 70);
};
