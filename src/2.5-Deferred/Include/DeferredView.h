#pragma once

#include "GBuffer.h"
#include "Deferred/Lights/PointLightsContainer.h"
#include "Deferred/Lights/DirectionalLightsContainer.h"
#include "GLWidget.hpp"
#include "Utils/FrameCounter.h"
#include "Utils/Time.h"
#include "SceneLogic/Camera/CameraView.h"
#include "Views/InspectorView.h"
#include "Views/SceneHierarchyView.h"
#include "SceneLogic/Scene.h"

#include <memory>

#include <QOpenGLShaderProgram>
#include <QOpenGLTextureBlitter>
#include <QOpenGLVertexArrayObject>

class Renderer;

class DeferredView final : public fgl::GLWidget {
public:
    DeferredView();

protected:
    void onInit() override;
    void onRender() override;
    void onResize(size_t width, size_t height) override;

private:
    std::unique_ptr<Scene> scene_;

    FieldsDrawer *debugWindow_;
    InspectorView *inspector_;
    SceneHierarchyView *sceneView_;
    FrameCounter frameCounter_;

    Deferred::PointLightsContainer* points_;
    Deferred::DirectionalLightsContainer* directionals_;

    std::shared_ptr<QOpenGLShaderProgram> geometryProgram_;
    std::shared_ptr<QOpenGLShaderProgram> nullForStencilTestProgram_;

    CameraView cameraView_;
    Time time_{};
    QColor clearColor_ = QColor(70, 70, 70);

    bool animated_ = true;

	GBuffer gBuffer_;
    int countSamplesFromGBuffer_ = 0;
    bool isNeedChangeSamples_ = false;
};
