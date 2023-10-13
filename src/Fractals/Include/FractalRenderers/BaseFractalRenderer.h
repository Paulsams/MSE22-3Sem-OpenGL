#pragma once

#include <QOpenGLShaderProgram>

namespace Fractals {
    class FractalView;

    struct BaseFractalRenderer {
        virtual void init(FractalView& view, QOpenGLShaderProgram* program) = 0;
        virtual void render(FractalView& view, QOpenGLShaderProgram* program, double deltaTime) = 0;

        virtual std::string getPathVertexShader() = 0;
        virtual std::string getPathFragmentShader() = 0;
        virtual QSizeF getAspectScreenToWorld(FractalView &view) = 0;

        float zoom = 1.0f;
        QVector2D viewPosition{0, 0};
    };
}
