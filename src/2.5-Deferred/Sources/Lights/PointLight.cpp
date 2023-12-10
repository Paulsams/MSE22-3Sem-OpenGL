#include "Lights/PointLight.h"
#include "Base/Include/SceneLogic/SceneNode.h"
#include "Base/Include/Views/FieldsDrawer.h"
#include "Base/Include/LoaderModels/LoaderModel.h"


void PointLight::init(const std::string &pathToSphere, QOpenGLExtraFunctions* funcs,
                      const std::shared_ptr<QOpenGLShaderProgram>& forStencilPass,
                      const std::shared_ptr<QOpenGLShaderProgram>& forLightPass) {
    forStencilPass_ = forStencilPass;
    forLightPass_ = forLightPass;
    funcs_ = funcs;

    stencilModel_ = LoaderModel::load(pathToSphere.c_str(), *funcs, forStencilPass);
    getNode().addChild(LoaderModel::load(pathToSphere.c_str(), *funcs, forLightPass));
}

void PointLight::setAllData(float distance, QVector3D ambient, QVector3D diffuse, QVector3D specular,
                float constant, float linear, float quadratic) {
    setDistance(distance);
    ambient_ = ambient;
    diffuse_ = diffuse;
    specular_ = specular;
    constant_ = constant;
    linear_ = linear;
    quadratic_ = quadratic;
}

float PointLight::getDistance() const {
    QVector3D scale = getNode().getTransform().getScale();
    return std::max(scale.x(), std::max(scale.y(), scale.z()));
}

void PointLight::setDistance(const float value) {
    getNode().getTransform().setScale(value, value, value);
    stencilModel_->getTransform().setScale(value, value, value);
}

void PointLight::onUpdate(float) {
    // TODO: нужна не локал позиция, а глобал
    const QVector3D currentPosition = getNode().getTransform().getPosition();
    if (position_ != currentPosition) {
        position_ = currentPosition;
        stencilModel_->getTransform().setPosition(position_);
    }
}

bool PointLight::drawValueInInspector(FieldsDrawer& drawer) {
    drawer.addSpinBox("Distance", getDistance(), 0.01f, 100.0f, 0.5f,
                      [this](const float newValue) { setDistance(newValue); });
    drawer.addColorPicker("Ambient", &ambient_);
    drawer.addColorPicker("Diffuse", &diffuse_);
    drawer.addColorPicker("Specular", &specular_);
    drawer.addSpinBox("Constant", &constant_, 0.0f, 10.0f, 0.1f);
    drawer.addSpinBox("Linear", &linear_, 0.0f, 10.0f, 0.1f);
    drawer.addSpinBox("Quadratic", &quadratic_, 0.0f, 10.0f, 0.1f);

    return true;
}

void PointLight::executeStencilPass(const QMatrix4x4& viewProjection) {
    forStencilPass_->link();
    forStencilPass_->bind();

    funcs_->glEnable(GL_DEPTH_TEST);
    funcs_->glDisable(GL_CULL_FACE);
    funcs_->glClear(GL_STENCIL_BUFFER_BIT);

    // Нам нужен тест трафарета, но мы хотим, что бы он всегда
    // успешно проходил. Важен только тест глубины.
    funcs_->glStencilFunc(GL_ALWAYS, 0, 0);

    funcs_->glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    funcs_->glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    auto modelMatrix = stencilModel_->getTransform().getModelMatrix();
    Renderer* renderer = stencilModel_->getChildren()[0]->getChildren()[0]->getChildren()[0]->getRenderer();
    renderer->draw(modelMatrix, viewProjection);

    funcs_->glStencilFunc(GL_ALWAYS, 0, 1);
    funcs_->glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    forStencilPass_->release();
}

void PointLight::executeLightPass(const QMatrix4x4 &viewProjection, const QVector3D &viewPosition, QSize sizeWindow) {
    forLightPass_->link();
    forLightPass_->bind();

    funcs_->glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

    funcs_->glDisable(GL_DEPTH_TEST);
    funcs_->glEnable(GL_BLEND);
    funcs_->glBlendEquation(GL_FUNC_ADD);
    funcs_->glBlendFunc(GL_ONE, GL_ONE);

    funcs_->glEnable(GL_CULL_FACE);
    funcs_->glCullFace(GL_FRONT);

    forLightPass_->setUniformValue("screen_size", sizeWindow);
    // TODO: нужно разделение на локальную позицию и глобальную
    forLightPass_->setUniformValue("global_position", getNode().getTransform().getPosition());
    forLightPass_->setUniformValue("constant", constant_);
    forLightPass_->setUniformValue("linear", linear_);
    forLightPass_->setUniformValue("quadratic", quadratic_);
    forLightPass_->setUniformValue("ambient", ambient_);
    forLightPass_->setUniformValue("diffuse", diffuse_);
    forLightPass_->setUniformValue("specular", specular_);
    forLightPass_->setUniformValue("power_specular", 128.0f);
    forLightPass_->setUniformValue("view_position", viewPosition);

    forLightPass_->setUniformValue("g_buffer_position", GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    forLightPass_->setUniformValue("g_buffer_color", GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    forLightPass_->setUniformValue("g_buffer_normal", GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);

    auto modelMatrix = getNode().getTransform().getModelMatrix();
    Renderer* renderer = getNode().getChildren()[0]->getChildren()[0]->getChildren()[0]->getChildren()[0]->getRenderer();
    renderer->draw(modelMatrix, viewProjection);

    forLightPass_->release();

    funcs_->glCullFace(GL_BACK);
    funcs_->glDisable(GL_BLEND);
}
