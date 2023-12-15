#pragma once

#include "SceneLogic/Scene.h"
#include "Views/FieldsDrawer.h"
#include "Views/InspectorView.h"
#include "Views/SceneHierarchyView.h"
#include "Utils/FrameCounter.h"
#include "GLWidget.hpp"
#include "Utils/Time.h"
#include "SceneLogic/Camera/CameraView.h"
#include "Deferred/Lights/PointLightsContainer.h"
#include "Deferred/Lights/DirectionalLightsContainer.h"
#include "GBuffer.h"

#include <memory>
#include <QOpenGLShaderProgram>

namespace SSAOImpl {
    class SSAOView : public fgl::GLWidget {
    public:
        SSAOView();

    protected:
        void onInit() override;

        void onRender() override;

        void onResize(size_t width, size_t height) override;

    private:
        void geometryPass(const QMatrix4x4& viewProjection, const QVector3D& viewPosition);
        void SSAOPass(const QMatrix4x4& projection);
        void blurPass();
        void lightsPass(const QMatrix4x4& viewProjection, const QVector3D& viewPosition);

        void updateKernels(int count);

        std::unique_ptr<Scene> scene_;

        FieldsDrawer* debugWindow_;
        InspectorView* inspector_;
        SceneHierarchyView* sceneView_;
        FrameCounter frameCounter_;

        Deferred::PointLightsContainer* points_;
        Deferred::DirectionalLightsContainer* directionals_;

        std::shared_ptr<QOpenGLShaderProgram> geometryProgram_;
        std::shared_ptr<QOpenGLShaderProgram> ssaoProgram_;
        std::shared_ptr<QOpenGLShaderProgram> blurProgram_;
        std::shared_ptr<QOpenGLShaderProgram> nullForStencilTestProgram_;

        CameraView cameraView_;
        Time time_{};

        bool animated_ = true;

        GBuffer gBuffer_;
        QOpenGLVertexArrayObject emptyVAO_;

        std::vector<QVector3D> kernelPoints_;
        int kernelPointsCount_;
        float sampleRadiusForSSAO_ = 1.7f;

        int indexOutputBuffer_ = GBuffer::GBUFFER_NUM_TEXTURES;
    };
}
