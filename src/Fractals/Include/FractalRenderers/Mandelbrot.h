#pragma once

#include <QVector2D>
#include <QOpenGLBuffer>
#include "BaseFractalRenderer.h"

namespace Fractals {
    class FractalView;

    class Mandelbrot : public BaseFractalRenderer {
    public:
        explicit Mandelbrot(float aspect)
            : aspect_(aspect) { }

        void init(FractalView& view, QOpenGLShaderProgram* program) override;
        void render(FractalView& view, QOpenGLShaderProgram* program, double deltaTime) override;

        QSizeF getAspectScreenToWorld(FractalView &view) override;

        std::string getPathVertexShader() override { return ":/Shaders/mandelbrot.vert"; }

        std::string getPathFragmentShader() override { return ":/Shaders/mandelbrot.frag"; }

    private:
        GLint matrixUniform_ = -1;
        GLint zoomUniform_ = -1;
        GLint viewPositionUniform_ = -1;
        GLint maxIterationsUniform_ = -1;

        const float aspect_;
        int maxIterations_ = 100.0f;
    };
}
