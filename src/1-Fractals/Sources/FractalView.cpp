#include "FractalView.h"

#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QScreen>
#include <QTimer>
#include <QHBoxLayout>

#include <array>
#include <memory>

#include "Base/Include/Views/FieldsDrawer.h"

namespace {
    constexpr float lengthPlane = 3.0f;

    constexpr std::array<GLfloat, 8u> vertices = {
            -lengthPlane, -lengthPlane,
            lengthPlane, -lengthPlane,
            -lengthPlane, lengthPlane,
            lengthPlane, lengthPlane,
    };
    constexpr std::array<GLuint, 6u> indices = {0, 1, 2, 2, 1, 3};

}// namespace

namespace Fractals {
    FractalView::FractalView(std::unique_ptr<BaseFractalRenderer> &&renderer,
                             std::unique_ptr<BaseMoveBehaviour> &&moveBehaviour)
            : debugWindow_(new FieldsDrawer(160, this)), renderer_(std::move(renderer)),
              moveBehaviour_(std::move(moveBehaviour)), frameCounter_(*debugWindow_, 1000.0f) {
        // TODO: потому что не хотелся добавлять бэкграунд почему-то
        auto* parentFormDebugWindow = new QWidget(this);
        debugWindow_->setParent(parentFormDebugWindow);

        parentFormDebugWindow->setStyleSheet("background-color: rgba(122, 122, 122, 200)");

        // TODO: на старом ещё нормально расстягивалось, а тут не хочет
        parentFormDebugWindow->setGeometry(10, 10, 400, 100);
        debugWindow_->setGeometry(5, 5, 390, 90);
        debugWindow_->setFontSize(18);
    }

    void FractalView::onInit() {
        timer_.start();

        // Configure shaders
        program_ = std::make_unique<QOpenGLShaderProgram>(this);
        program_->addShaderFromSourceFile(QOpenGLShader::Vertex, renderer_->getPathVertexShader().c_str());
        program_->addShaderFromSourceFile(QOpenGLShader::Fragment, renderer_->getPathFragmentShader().c_str());
        program_->link();

        // Create VAO object
        vao_.create();
        vao_.bind();

        // Create VBO
        vbo_.create();
        vbo_.bind();
        vbo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
        vbo_.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(GLfloat)));

        // Create IBO
        ibo_.create();
        ibo_.bind();
        ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
        ibo_.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(GLuint)));

        // Bind attributes
        program_->bind();

        program_->enableAttributeArray(0);
        program_->setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(2 * sizeof(GLfloat)));

        renderer_->init(*this, program_.get());

        // Release all
        program_->release();

        vao_.release();

        ibo_.release();
        vbo_.release();

        // Uncomment to enable depth test and face culling
        // glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);

        // Clear all FBO buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void FractalView::onRender() {
        const qint64 currentTime = timer_.elapsed();
        const double deltaTime = static_cast<double>(currentTime - lastTimeRender_) / 1000.0;
        lastTimeRender_ = currentTime;

        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        moveBehaviour_->update(*this, *renderer_, deltaTime);

        // Bind VAO and shader program
        program_->bind();
        vao_.bind();

        renderer_->render(*this, program_.get(), deltaTime);

        // Draw
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Release VAO and shader program
        vao_.release();
        program_->release();

        // Update frame counter
        frameCounter_.update();
        update();
    }

    void FractalView::mousePressEvent(QMouseEvent *mouseEvent) {
        moveBehaviour_->mousePressEvent(*this, *renderer_, mouseEvent);
    }

    void FractalView::mouseMoveEvent(QMouseEvent *mouseEvent) {
        moveBehaviour_->mouseMoveEvent(*this, *renderer_, mouseEvent);
    }

    void FractalView::mouseReleaseEvent(QMouseEvent *mouseEvent) {
        moveBehaviour_->mouseReleaseEvent(*this, *renderer_, mouseEvent);
    }

    void FractalView::wheelEvent(QWheelEvent *wheelEvent) {
        constexpr float speedStep = 0.1f;

        float directionStep = wheelEvent->angleDelta().y();
        directionStep = (directionStep > 0) ? 1.0f : ((directionStep < 0) ? -1.0f : 0.0f);
        float oldZoom = renderer_->zoom;
        float step = directionStep * speedStep * renderer_->zoom;
        renderer_->zoom = std::max(renderer_->zoom + step, 1.0f);

        float coefficientStep = (renderer_->zoom - oldZoom) / step;
        if (coefficientStep <= std::numeric_limits<float>::epsilon())
            return;

        auto sizeWindow = QPointF(width(), height());
        QVector2D difference = QVector2D(wheelEvent->position() - sizeWindow / 2.0f);
        difference.setY(-difference.y());

        const QSizeF aspect = renderer_->getAspectScreenToWorld(*this);
        auto shift = difference / QVector2D(width(), height()) /
                     renderer_->zoom * QVector2D(aspect.width(), aspect.height()) *
                     directionStep * speedStep * coefficientStep;
        renderer_->viewPosition += shift;
    }

    void FractalView::onResize(size_t width, size_t height) {
        // Configure viewport
        glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));
    }

    void FractalView::closeEvent(QCloseEvent *event) {
        vao_.destroy();
        QOpenGLWidget::closeEvent(event);
    }
}
