#pragma once

#include "GBuffer.h"
#include "Base/Include/GLWidget.hpp"
#include "Base/Include/Utils/FrameCounter.h"
#include "Base/Include/Utils/Time.h"
#include "Base/Include/SceneLogic/Camera/CameraView.h"
#include "Base/Include/Views/InspectorView.h"
#include "Base/Include/Views/SceneHierarchyView.h"
#include "Base/Include/SceneLogic/Scene.h"

#include <memory>

#include <QOpenGLShaderProgram>
#include <QOpenGLTextureBlitter>

class SSAOView final : public fgl::GLWidget {
public:
    SSAOView();

protected:
    void onInit() override;
    void onRender() override;
    void onResize(size_t width, size_t height) override;

private:
	void SSAOView::pointLightsPass(const QMatrix4x4 & viewProjection);

    std::unique_ptr<Scene> scene_;

    FieldsDrawer *debugWindow_;
    InspectorView *inspector_;
    SceneHierarchyView *sceneView_;
    FrameCounter frameCounter_;

    std::shared_ptr<QOpenGLShaderProgram> geometryProgram_;
    std::shared_ptr<QOpenGLShaderProgram> pointLightsProgram_;
    std::shared_ptr<QOpenGLShaderProgram> spotLightsProgram_;
    std::shared_ptr<QOpenGLShaderProgram> direcionalLightsProgram_;

    CameraView cameraView_;
    Time time_{};
    QColor clearColor_ = QColor(70, 70, 70);

    bool animated_ = true;

	GBuffer gBuffer_;
    int countSamplesFromGBuffer_ = 0;
    bool isNeedChangeSamples_ = false;

	std::shared_ptr<SceneNode> _shpereForPointLight;
    QOpenGLTextureBlitter blitter_;
};
