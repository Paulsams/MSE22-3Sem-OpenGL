#include <QMatrix4x4>
#include <QSlider>
#include <QOpenGLShaderProgram>

#include "FractalRenderers/Mandelbrot.h"
#include "FractalView.h"
#include "Base/DebugWindow.h"

namespace Fractals {
    void Mandelbrot::init(FractalView &view, QOpenGLShaderProgram *program) {
        matrixUniform_ = program->uniformLocation("matrix");
        zoomUniform_ = program->uniformLocation("zoom");
        viewPositionUniform_ = program->uniformLocation("viewPosition");
        maxIterationsUniform_ = program->uniformLocation("maxIterations");

        auto iterCounter = view.getDebugContainer()->addSlider("Max Iterations", "Max Iterations",
                                                               maxIterations_, 50, 300);

        QWidget::connect(iterCounter, &QSlider::valueChanged, &view, [this](int newValue) {
            maxIterations_ = newValue;
        });
    }

    void Mandelbrot::render(FractalView &view, QOpenGLShaderProgram *program, double) {
        const auto aspect = getAspectScreenToWorld(view);
        const auto halfAspect = aspect / 2.0f;

        // Calculate MVP matrix
        QMatrix4x4 matrix;
        matrix.ortho(-halfAspect.width(),
                     halfAspect.width(),
                     -halfAspect.height(),
                     halfAspect.height(),
                     0.1f, 100.0f);
        matrix.translate(0.0f, 0.0f, -1.0f);

        // Update uniform value
        program->setUniformValue(matrixUniform_, matrix);
        program->setUniformValue(zoomUniform_, zoom);
        program->setUniformValue(viewPositionUniform_, viewPosition);
        program->setUniformValue(maxIterationsUniform_, maxIterations_);
    }

    QSizeF Mandelbrot::getAspectScreenToWorld(FractalView &view) {
        return {view.width() / aspect_, view.height() / aspect_};
    }
}
