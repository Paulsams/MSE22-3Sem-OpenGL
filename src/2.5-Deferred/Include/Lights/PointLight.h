#pragma once

#include "Base/Include/SceneLogic/BaseComponent.h"
#include "GBuffer.h"

#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <memory>
#include <QOpenGLExtraFunctions>

class PointLight : public BaseComponent {
public:
    void init(const std::string &pathToSphere, QOpenGLExtraFunctions* funcs,
              const std::shared_ptr<QOpenGLShaderProgram>& forStencilPass,
              const std::shared_ptr<QOpenGLShaderProgram>& forLightPass);

    void setAllData(float distance, QVector3D ambient, QVector3D diffuse, QVector3D specular,
                    float constant, float linear, float quadratic);

    [[nodiscard]] float getDistance() const;

    void setDistance(float value);

    [[nodiscard]] QVector3D getAmbient() const { return ambient_; }

    void setAmbient(QVector3D value) { ambient_ = value; }

    [[nodiscard]] QVector3D getDiffuse() const { return diffuse_; }

    void setDiffuse(QVector3D value) { diffuse_ = value; }

    [[nodiscard]] QVector3D getSpecular() const { return specular_; }

    void setSpecular(QVector3D value) { specular_ = value; }

    [[nodiscard]] float getConstant() const { return constant_; }

    void setConstant(float value) { constant_ = value; }

    [[nodiscard]] float getLinear() const { return linear_; }

    void setLinear(float value) { linear_ = value; }

    [[nodiscard]] float getQuadratic() const { return quadratic_; }

    void setQuadratic(float value) { quadratic_ = value; }

    const char* getComponentName() override { return "Point Light"; }

    // TODO: сделать френдами или через интерфейс спрятать эти методы
    void executeStencilPass(const QMatrix4x4 &viewProjection);

    void executeLightPass(const QMatrix4x4 &viewProjection, const QVector3D& viewPosition, QSize sizeWindow);

    static constexpr const char* defaultNameInHierarchy = "Point";

protected:
    bool drawValueInInspector(FieldsDrawer& drawer) override;
    void onUpdate(float) override;

private:
    QVector3D position_{};
    QVector3D ambient_{};
    QVector3D diffuse_{};
    QVector3D specular_{};

    float constant_{};
    float linear_{};
    float quadratic_{};

    std::shared_ptr<SceneNode> stencilModel_;

    QOpenGLExtraFunctions* funcs_;

    std::shared_ptr<QOpenGLShaderProgram> forStencilPass_;
    std::shared_ptr<QOpenGLShaderProgram> forLightPass_;
};
