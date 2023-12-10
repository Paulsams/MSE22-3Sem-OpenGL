#include "Lights/DirectionalLightsContainer.h"

DirectionalLightsContainer::DirectionalLightsContainer(QOpenGLExtraFunctions* funcs)
        : lightPassProgram_(std::make_shared<QOpenGLShaderProgram>(this))
        , funcs_(funcs) {
    lightPassProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/directionalLight.vert");
    lightPassProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/directionalLight.frag");

    fullscreenVAO_.create();
}

DirectionalLight &DirectionalLightsContainer::addLight() {
    DirectionalLight& light = BaseLightsContainer::addLight();
    light.init(funcs_, &fullscreenVAO_, lightPassProgram_);
    return light;
}

void DirectionalLightsContainer::executeLightPasses(const QVector3D& viewPosition, GBuffer& gBuffer, QSize sizeWindow) {
    funcs_->glDisable(GL_DEPTH_TEST);
    funcs_->glEnable(GL_BLEND);
    funcs_->glBlendEquation(GL_FUNC_ADD);
    funcs_->glBlendFunc(GL_ONE, GL_ONE);

    for (const auto &light: lights_) {
        gBuffer.bindForLightPass();
        light->executeLightPass(viewPosition, sizeWindow);
    }

    funcs_->glDisable(GL_BLEND);
}
