#pragma once

#include "PointLight.h"
#include "BaseLightsContainer.h"

#include <memory>

namespace Deferred {
    class PointLightsContainer : public BaseLightsContainer<PointLight> {
    public:
        explicit PointLightsContainer(std::vector<std::pair<const char*, int>>& texturesBind,
                                      std::shared_ptr<QOpenGLShaderProgram>& stencilTestPassProgram,
                                      QOpenGLExtraFunctions* funcs);

        PointLight& addLight() override;

        void executeStencilAndLightPasses(
                const QMatrix4x4& view,
                const QMatrix4x4& viewProjection,
                const QVector3D& viewPosition,
                IGBuffer& gBuffer,
                QSize sizeWindow
        );

    private:
        std::shared_ptr<QOpenGLShaderProgram> stencilTestPassProgram_;
        std::shared_ptr<QOpenGLShaderProgram> lightPassProgram_;

        QOpenGLExtraFunctions* funcs_;
    };
}
