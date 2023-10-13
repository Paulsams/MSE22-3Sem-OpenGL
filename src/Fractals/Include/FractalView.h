#pragma once

#include "FractalRenderers/Mandelbrot.h"
#include "MoveFractalBehaviours/MoveNearWindowSidesBehaviour.h"
#include "MoveFractalBehaviours/DragBehaviour.h"
#include "DebugWindow.h"

#include "Base/GLWindow.hpp"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QQuaternion>
#include <QVector2D>
#include <QVector3D>
#include <QElapsedTimer>
#include <QPainter>
#include <QBackingStore>
#include <QOpenGLWidget>
#include <QLayout>

#include <memory>

class DebugWindow;

namespace Fractals {
    class FractalView final : public QOpenGLWidget, protected QOpenGLFunctions {
    public:
        FractalView();
        ~FractalView();

        void initializeGL() override;
        void paintGL() override;
        void resizeGL(int width, int height) override;

        DebugWindow* getDebugContainer() { return debugWindow_; }

    protected:
        void mousePressEvent(QMouseEvent *mouseEvent) override;
        void mouseMoveEvent(QMouseEvent *mouseEvent) override;
        void mouseReleaseEvent(QMouseEvent *mouseEvent) override;
        void wheelEvent(QWheelEvent *wheelEvent) override;

        void closeEvent(QCloseEvent *event) override;

    private:
        DebugWindow* debugWindow_;

        QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
        QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
        QOpenGLVertexArrayObject vao_;

        std::unique_ptr<BaseFractalRenderer> renderer_ = std::make_unique<Mandelbrot>(400.0f);
        std::unique_ptr<BaseMoveBehaviour> moveBehaviour_ = std::make_unique<DragBehaviour>();

        std::unique_ptr<QOpenGLShaderProgram> program_ = nullptr;

        size_t frame_ = 0;

        qint64 lastTimeRender_ = 0;
        QElapsedTimer timer_{};
    };
}
