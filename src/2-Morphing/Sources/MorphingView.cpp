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
          modelHolder_("../Models/Cube.glb", *this, *program_),
          cameraView_(Camera{0.0f, 60.0f, 0.1f, 1000.0f}, this) {
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
    model_.scale(10.0f);

    Camera& camera = cameraView_.getCamera();
    camera.move(cameraView_.getDirectionMoveCamera() * static_cast<float>(speedCamera * time_.getDeltaTime()));

    auto vp = camera.getProjectionMatrix() * camera.getViewMatrix();

    // Bind VAO and shader program
    program_->bind();

    program_->setUniformValue("ambient_strength", 0.1f);
    program_->setUniformValue("light_color", 1.0f, 1.0f, 1.0f);
    program_->setUniformValue("light_position", 0.0f, 0.0f, 10.0f);
    program_->setUniformValue("view_position", camera.getPosition());
    program_->setUniformValue("specular_strength", 0.5f);
    program_->setUniformValue("power_specular", 128);

    // Try Draw
    modelHolder_.draw(model_, vp);

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
