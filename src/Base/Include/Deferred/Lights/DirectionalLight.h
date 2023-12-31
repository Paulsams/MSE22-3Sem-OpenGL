#pragma once

#include "SceneLogic/BaseComponent.h"
#include "Deferred/IGBuffer.h"

#include <QVector3D>
#include <QSize>
#include <QOpenGLExtraFunctions>
#include <memory>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

namespace Deferred {
    class DirectionalLight : public BaseComponent {
    public:
        DirectionalLight(const std::vector<std::pair<const char*, int>>& texturesBind)
            : texturesBind_(texturesBind) { }

        void init(QOpenGLExtraFunctions *funcs,
                  QOpenGLVertexArrayObject *fullscreenVAO,
                  const std::shared_ptr<QOpenGLShaderProgram> &lightPassProgram);

        void setAllData(QVector3D ambient, QVector3D diffuse, QVector3D specular);

        [[nodiscard]] QVector3D getAmbient() const { return ambient_; }

        void setAmbient(QVector3D value) { ambient_ = value; }

        [[nodiscard]] QVector3D getDiffuse() const { return diffuse_; }

        void setDiffuse(QVector3D value) { diffuse_ = value; }

        [[nodiscard]] QVector3D getSpecular() const { return specular_; }

        void setSpecular(QVector3D value) { specular_ = value; }

        const char *getComponentName() override { return "Directional Light"; }

        void executeLightPass(const QVector3D &viewPosition, IGBuffer& gBuffer, QSize sizeWindow);

        static constexpr const char *defaultNameInHierarchy = "Directional";

    protected:
        bool drawValueInInspector(FieldsDrawer &drawer) override;

    private:
        QVector3D ambient_{};
        QVector3D diffuse_{};
        QVector3D specular_{};

        QOpenGLExtraFunctions *funcs_;

        std::shared_ptr<QOpenGLShaderProgram> lightPassProgram_;
        QOpenGLVertexArrayObject *fullscreenVAO_;

        const std::vector<std::pair<const char*, int>>& texturesBind_;
    };
}
