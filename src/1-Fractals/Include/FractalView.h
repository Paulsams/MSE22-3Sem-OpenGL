#pragma once

#include "../../Base/Include/GLWidget.hpp"
#include "FractalRenderers/BaseFractalRenderer.h"
#include "MoveFractalBehaviours/BaseMoveBehaviour.h"
#include "../../Base/Include/FrameCounter.h"

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
    class FractalView final : public fgl::GLWidget {
    public:
        FractalView(std::unique_ptr<BaseFractalRenderer> &&renderer,
                    std::unique_ptr<BaseMoveBehaviour> &&moveBehaviour);

        void onInit() override;

        void onRender() override;

        void onResize(size_t width, size_t height) override;

        DebugWindow *getDebugContainer() { return debugWindow_; }

    protected:
        void mousePressEvent(QMouseEvent *mouseEvent) override;

        void mouseMoveEvent(QMouseEvent *mouseEvent) override;

        void mouseReleaseEvent(QMouseEvent *mouseEvent) override;

        void wheelEvent(QWheelEvent *wheelEvent) override;

        void closeEvent(QCloseEvent *event) override;

    private:
        DebugWindow *debugWindow_;

        QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
        QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
        QOpenGLVertexArrayObject vao_;

        std::unique_ptr<BaseFractalRenderer> renderer_;
        std::unique_ptr<BaseMoveBehaviour> moveBehaviour_;

        std::unique_ptr<QOpenGLShaderProgram> program_ = nullptr;
        FrameCounter frameCounter_;

        qint64 lastTimeRender_ = 0;
        QElapsedTimer timer_{};
    };
}
