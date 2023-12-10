#pragma once

#include "PointLight.h"
#include "BaseLightsContainer.h"

#include <memory>

class PointLightsContainer : public BaseLightsContainer<PointLight> {
public:
    explicit PointLightsContainer(std::shared_ptr<QOpenGLShaderProgram>& stencilTestPassProgram,
                                  QOpenGLExtraFunctions* funcs);

    PointLight& addLight() override;

    void executeStencilAndLightPasses(
            const QMatrix4x4& viewProjection,
            const QVector3D& viewPosition,
            GBuffer& gBuffer,
            QSize sizeWindow
    );

private:
    std::shared_ptr<QOpenGLShaderProgram> stencilTestPassProgram_;
    std::shared_ptr<QOpenGLShaderProgram> lightPassProgram_;

    QOpenGLExtraFunctions* funcs_;
};

