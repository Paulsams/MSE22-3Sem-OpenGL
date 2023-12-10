#include "Lights/DirectionalLight.h"
#include "Base/Include/Views/FieldsDrawer.h"
#include "Base/Include/SceneLogic/SceneNode.h"
#include "GBuffer.h"

void DirectionalLight::init(QOpenGLExtraFunctions* funcs,
                            QOpenGLVertexArrayObject* fullscreenVAO,
                            const std::shared_ptr<QOpenGLShaderProgram>& lightPassProgram) {
    funcs_ = funcs;
    lightPassProgram_ = lightPassProgram;
    fullscreenVAO_ = fullscreenVAO;
}

void DirectionalLight::setAllData(QVector3D ambient, QVector3D diffuse, QVector3D specular) {
    ambient_ = ambient;
    diffuse_ = diffuse;
    specular_ = specular;
}

void DirectionalLight::executeLightPass(const QVector3D& viewPosition, QSize sizeWindow) {
    lightPassProgram_->link();
    lightPassProgram_->bind();

    lightPassProgram_->setUniformValue("power_specular", 128.0f);
    lightPassProgram_->setUniformValue("view_position", viewPosition);
    lightPassProgram_->setUniformValue("screen_size", sizeWindow);

    lightPassProgram_->setUniformValue("direction", getNode().getTransform().getRotation() * QVector3D(0.0f, 1.0f, 0.0f));
    lightPassProgram_->setUniformValue("ambient", ambient_);
    lightPassProgram_->setUniformValue("diffuse", diffuse_);
    lightPassProgram_->setUniformValue("specular", specular_);

    lightPassProgram_->setUniformValue("g_buffer_position", GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    lightPassProgram_->setUniformValue("g_buffer_color", GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    lightPassProgram_->setUniformValue("g_buffer_normal", GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);

    fullscreenVAO_->bind();
    funcs_->glDrawArrays(GL_TRIANGLES, 0, 3);
    fullscreenVAO_->release();

    lightPassProgram_->release();
}

bool DirectionalLight::drawValueInInspector(FieldsDrawer& drawer) {
    drawer.addColorPicker("Ambient", &ambient_);
    drawer.addColorPicker("Diffuse", &diffuse_);
    drawer.addColorPicker("Specular", &specular_);

    return true;
}
