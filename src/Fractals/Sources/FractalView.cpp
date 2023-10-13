#include "Fractals/Include/FractalView.h"

#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QScreen>
#include <QTimer>
#include <QHBoxLayout>

#include <array>
#include <memory>

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
    FractalView::FractalView()
        : debugWindow_(new DebugWindow(this)) { }

    FractalView::~FractalView() { }

    void FractalView::initializeGL() {
        initializeOpenGLFunctions();

        auto *updater = new QTimer(this);
        connect(updater, &QTimer::timeout, this, [this]() { this->update(); });
        updater->start(1.0 / screen()->refreshRate());

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

    void FractalView::paintGL() {
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

        // Increment frame counter
        ++frame_;
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

        float step = wheelEvent->angleDelta().y();
        step = (step > 0) ? 1.0f : ((step < 0) ? -1.0f : 0.0f);
        // TODO: в идеале бы сделать зум в точку, а не в центр
        renderer_->zoom = std::max(renderer_->zoom + step * speedStep * renderer_->zoom, 1.0f);
    }

    void FractalView::resizeGL(int, int) {
        // Configure viewport
        const auto retinaScale = devicePixelRatio();
        glViewport(0, 0, static_cast<GLint>(width() * retinaScale),
                   static_cast<GLint>(height() * retinaScale));
    }

    void FractalView::closeEvent(QCloseEvent* event) {
        vao_.destroy();
        QOpenGLWidget::closeEvent(event);
    }
}
