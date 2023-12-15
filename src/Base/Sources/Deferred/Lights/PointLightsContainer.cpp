#include "Base/Include/Deferred/Lights/PointLightsContainer.h"

namespace Deferred {
    PointLightsContainer::PointLightsContainer(std::vector<std::pair<const char*, int>>& texturesBind,
                                               std::shared_ptr<QOpenGLShaderProgram>& stencilTestPassProgram,
                                               QOpenGLExtraFunctions* funcs)
            : BaseLightsContainer(texturesBind), stencilTestPassProgram_(stencilTestPassProgram),
              lightPassProgram_(std::make_shared<QOpenGLShaderProgram>(this)), funcs_(funcs) {
        lightPassProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/light.vert");
        lightPassProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/pointLight.frag");
    }

    PointLight &PointLightsContainer::addLight() {
        PointLight &light = BaseLightsContainer::addLight();
        light.init("../Models/Sphere.glb", funcs_, stencilTestPassProgram_, lightPassProgram_);
        return light;
    }

    void PointLightsContainer::executeStencilAndLightPasses(
            const QMatrix4x4& view,
            const QMatrix4x4& viewProjection,
            const QVector3D& viewPosition,
            IGBuffer& gBuffer,
            QSize sizeWindow) {
        for (const auto& light: lights_) {
            gBuffer.bindForStencilPass();
            light->executeStencilPass(view, viewProjection);

            gBuffer.bindForLightPass();
            light->executeLightPass(view, viewProjection, viewPosition, gBuffer, sizeWindow);
        }
    }
}
