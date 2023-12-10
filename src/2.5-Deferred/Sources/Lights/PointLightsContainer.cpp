#include "Lights/PointLightsContainer.h"

PointLightsContainer::PointLightsContainer(std::shared_ptr<QOpenGLShaderProgram> &stencilTestPassProgram,
                                           QOpenGLExtraFunctions* funcs)
        : stencilTestPassProgram_(stencilTestPassProgram)
        , lightPassProgram_(std::make_shared<QOpenGLShaderProgram>(this))
        , funcs_(funcs) {
    lightPassProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/light.vert");
    lightPassProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/pointLight.frag");
}

PointLight& PointLightsContainer::addLight() {
    PointLight& light = BaseLightsContainer::addLight();
    light.init("../Models/Sphere.glb", funcs_, stencilTestPassProgram_, lightPassProgram_);
    return light;
}

void PointLightsContainer::executeStencilAndLightPasses(
        const QMatrix4x4& viewProjection,
        const QVector3D& viewPosition,
        GBuffer& gBuffer,
        QSize sizeWindow) {
    for (const auto &light: lights_) {
        gBuffer.bindForStencilPass();
        light->executeStencilPass(viewProjection);

        gBuffer.bindForLightPass();
        light->executeLightPass(viewProjection, viewPosition, sizeWindow);
    }
}
