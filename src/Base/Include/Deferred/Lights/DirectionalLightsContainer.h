#pragma once

#include "BaseLightsContainer.h"
#include "DirectionalLight.h"

namespace Deferred {
    class DirectionalLightsContainer : public BaseLightsContainer<DirectionalLight> {
    public:
        DirectionalLightsContainer(std::vector<std::pair<const char*, int>>& texturesBind,
                                   QOpenGLExtraFunctions* funcs);

        DirectionalLight& addLight() override;

        void executeLightPasses(
                const QVector3D &viewPosition,
                IGBuffer& gBuffer,
                QSize sizeWindow
        );

    private:
        QOpenGLVertexArrayObject fullscreenVAO_;
        std::shared_ptr<QOpenGLShaderProgram> lightPassProgram_;

        QOpenGLExtraFunctions* funcs_;
    };
}
