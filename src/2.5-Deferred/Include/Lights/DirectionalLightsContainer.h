#pragma once

#include "Lights/BaseLightsContainer.h"
#include "DirectionalLight.h"

class DirectionalLightsContainer : public BaseLightsContainer<DirectionalLight> {
public:
    DirectionalLightsContainer(QOpenGLExtraFunctions* funcs);

    DirectionalLight& addLight() override;

    void executeLightPasses(
            const QVector3D& viewPosition,
            GBuffer& gBuffer,
            QSize sizeWindow
    );

private:
    QOpenGLVertexArrayObject fullscreenVAO_;
    std::shared_ptr<QOpenGLShaderProgram> lightPassProgram_;

    QOpenGLExtraFunctions* funcs_;
};
