#include "2-Morphing/Include/MorphingView.h"

#include <QLabel>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QApplication>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "tinygltf/tiny_gltf.h"

MorphingView::MorphingView() noexcept
        : debugWindow_(new DebugWindow(this)), frameCounter_(*debugWindow_, 1000.0f),
          program_(std::make_unique<QOpenGLShaderProgram>(this)),
          modelHolder_("../Models/chess.glb", *this, *program_),
          cameraView_(Camera{0.0f}, this) {
}

MorphingView::~MorphingView() {
    {
        // Free resources with context bounded.
        const auto guard = bindContext();
        texture_.reset();
        program_.reset();
    }
}

void MorphingView::onInit() {
    // Configure shaders
    program_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/diffuse.vert");
    program_->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                      ":/Shaders/diffuse.frag");
    program_->link();

    texture_ = std::make_unique<QOpenGLTexture>(QImage(":/Textures/voronoi.png"));
    texture_->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    texture_->setWrapMode(QOpenGLTexture::WrapMode::Repeat);

    // Bind attributes
    program_->bind();

    modelHolder_.init(program_->uniformLocation("mvp"));

    // Release all
    program_->release();

    // Еnable depth test and face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Clear all FBO buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MorphingView::onRender() {
    const double speedCamera = 10.0f;

    time_.update();

    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate MVP matrix
    model_.setToIdentity();
    model_.translate(0, 0, -10);
    model_.scale(0.1f);

    Camera& camera = cameraView_.getCamera();
    camera.move(cameraView_.getDirectionMoveCamera() * static_cast<float>(speedCamera * time_.getDeltaTime()));

    auto mvp = camera.getProjectionMatrix() * camera.getViewMatrix() * model_;

    // Bind VAO and shader program
    program_->bind();

    // Try Draw
    modelHolder_.draw(mvp);

    // Release VAO and shader program
    program_->release();

    frameCounter_.update();

    // Request redraw if animated
    if (animated_) {
        update();
    }
}

void MorphingView::onResize(const size_t width, const size_t height) {
    // Configure viewport
    glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));

    cameraView_.getCamera().setAspect(static_cast<float>(width) / static_cast<float>(height));
}
