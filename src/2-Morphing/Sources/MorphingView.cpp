#include "MorphingView.h"
#include "Scene.h"
#include "LoaderModel.h"
#include "MoverOnArrows.h"

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QApplication>
#include <memory>
#include <utility>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "tinygltf/tiny_gltf.h"

struct alignas(16) DirectionalLightUniformData {
    alignas(16) QVector3D direction;
    alignas(16) QVector3D color;
    float ambientStrength{};
};

struct alignas(16) PointLightUniformData {
    alignas(16) QVector3D position;
    float distance;

    alignas(16) QVector3D ambient;
    // float constant;

    alignas(16) QVector3D diffuse;
    // float linear;

    alignas(16) QVector3D specular;
    // float quadratic;
};

MorphingView::MorphingView() noexcept
    : scene_(std::make_unique<Scene>()),
      nodeDirectionalLight_(nullptr),
      debugWindow_(new DebugWindow(this)),
      frameCounter_(*debugWindow_, 1000.0f),
      program_(std::make_shared<QOpenGLShaderProgram>(this)),
      pointLights(*this),
      spotLights(*this),
      directionalLight(*this),
      cameraView_(Camera{0.0f, 60.0f, 0.1f, 100.0f}, this) {
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

    const auto nodeWithLoadModel = LoaderModel::load("../Models/Chess.glb", *this, program_);
    scene_->getRootNode().addChild(nodeWithLoadModel);

    nodeDirectionalLight_ = LoaderModel::load("../Models/Sphere.glb", *this, program_);

    {
        const GLuint blockIndex = glGetUniformBlockIndex(program_->programId(),
                                                         "DirectionalLight");
        glUniformBlockBinding(program_->programId(), blockIndex, 0);
        GLint blockSize;
        glGetActiveUniformBlockiv(program_->programId(), blockIndex,
                                  GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

        directionalLight.create();
        directionalLight.bind();

        directionalLight.allocate(blockSize);

        const DirectionalLightUniformData light = {
            QVector3D(0.0f, 1.0f, -1.0f).normalized(),
            {0.5f, 1.0f, 0.5f},
            0.2f
        };

        directionalLight.setSubData(0, sizeof(DirectionalLightUniformData), &light);

        directionalLight.release();
    }

    {
        // const GLuint blockIndex = glGetUniformBlockIndex(program_->programId(),
        //                                          "PointsLights");
        // glUniformBlockBinding(program_->programId(), blockIndex, 0);
        // GLint blockSize;
        // glGetActiveUniformBlockiv(program_->programId(), blockIndex,
        //                           GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
        //
        // // // Query for the offsets of each block variable
        // // const GLchar* names[2] = {"points[0].position", "numPointsLights"};
        // // GLuint indices[2];
        // // glGetUniformIndices(program_->programId(), 2, names, indices);
        // //
        // // if (indices[0] == GL_INVALID_INDEX || indices[1] == GL_INVALID_INDEX) {
        // //     printf("ERR: %s\n", "Indexes are not loaded correct");
        // // }
        // //
        // // GLint offset[2];
        // // glGetActiveUniformsiv(program_->programId(), 2, indices,
        // //                       GL_UNIFORM_OFFSET, offset);
        //
        // pointLights.create();
        // pointLights.bind();
        //
        // pointLights.allocate(blockSize);
        //
        // constexpr int countLights = 2;
        // constexpr PointLightUniformData lights[countLights] = {
        //     {
        //         {0.0f, 7.0f, -10.0f},
        //         2.0f,
        //         {0.5f, 1.0f, 0.5f},
        //         {0.5f, 1.0f, 0.5f},
        //         {0.5f, 1.0f, 0.5f},
        //     },
        //     {
        //         {0.0f, 7.0f, 10.0f},
        //         3.0f,
        //         {1.0f, 0.5f, 0.8f},
        //         {1.0f, 0.5f, 0.8f},
        //         {1.0f, 0.5f, 0.8f},
        //     },
        // };
        //
        // pointLights.setSubData(0, sizeof(PointLightUniformData) * countLights, lights);
        // pointLights.setSubData(sizeof(PointLightUniformData) * countLights, sizeof(GLfloat), &countLights);
        //
        // pointLights.release();
    }

    // GLuint blockIndex = glGetUniformBlockIndex(program_->programId(),
    //                                            "PointsLights");
    //
    // glUniformBlockBinding(program_->programId(), blockIndex, 0);
    //
    // GLint blockSize;
    // glGetActiveUniformBlockiv(program_->programId(), blockIndex,
    //                           GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
    // // Query for the offsets of each block variable
    // const GLchar* names[2] = {"points[0].position", "numPointsLights"};
    // GLuint indices[2];
    // glGetUniformIndices(program_->programId(), 2, names, indices);
    //
    // if (indices[0] == GL_INVALID_INDEX || indices[1] == GL_INVALID_INDEX) {
    //     printf("ERR: %s\n", "Indexes are not loaded correct");
    // }
    //
    // GLint offset[2];
    // glGetActiveUniformsiv(program_->programId(), 2, indices,
    //                       GL_UNIFORM_OFFSET, offset);
    //
    // unsigned int ubo;
    // glGenBuffers(1, &ubo);
    // glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    // glBufferData(GL_UNIFORM_BUFFER, blockSize, nullptr, GL_STATIC_DRAW);
    // glBindBuffer(GL_UNIFORM_BUFFER, 0);
    //
    // glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, blockSize);
    //
    // constexpr int countLights = 2;
    // const DirectionalLightUniformData lights[countLights] = {
    //     {
    //         QVector3D(0.0f, 1.0f, -1.0f).normalized(),
    //         {0.5f, 1.0f, 0.5f}, 0.2f
    //     },
    //     {
    //         QVector3D(0.0f, 1.0f, 1.0f).normalized(),
    //         {1.0f, 0.5f, 0.8f}, 0.2f
    //     },
    // };
    //
    // glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    // glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirectionalLightUniformData) * countLights, lights);
    // glBufferSubData(GL_UNIFORM_BUFFER, sizeof(DirectionalLightUniformData) * countLights, sizeof(GLfloat),
    //                 &countLights);
    // glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //        GLuint blockIndex = glGetUniformBlockIndex(program_->programId(),
    //                                                   "DirectionalLights");
    //
    //        GLint blockSize;
    //        glGetActiveUniformBlockiv(program_->programId(), blockIndex,
    //                                  GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
    //
    //        GLubyte *blockBuffer = (GLubyte *) malloc(blockSize);
    //
    //        // Query for the offsets of each block variable
    //        const GLchar *names[] = {"light_color", "light_position", "ambient_strength"};
    //        GLuint indices[3];
    //        glGetUniformIndices(program_->programId(), 3, names, indices);
    //
    //        GLint offset[3];
    //        glGetActiveUniformsiv(program_->programId(), 3, indices,
    //                              GL_UNIFORM_OFFSET, offset);
    //
    //        constexpr int countLights = 2;
    //        QVector3D light_position[countLights] =
    //                {{0.0f, 7.0f, -10.0f},
    //                 {0.0f, 7.0f, 10.0f}};
    //        QVector3D light_color[countLights] =
    //                {{1.0f, 1.0f, 1.0f},
    //                 {1.0f, 1.0f, 1.0f}};
    //        GLfloat ambient_strength[countLights] =
    //                {0.2f, 0.2f};
    //
    //        memcpy(blockBuffer + offset[0], light_position,
    //               3 * sizeof(GLfloat) * countLights);
    //        memcpy(blockBuffer + offset[1], light_color,
    //               3 * sizeof(GLfloat) * countLights);
    //        memcpy(blockBuffer + offset[2], ambient_strength,
    //               sizeof(GLfloat) * countLights);
    //
    //        GLuint uboHandle;
    //        glGenBuffers(1, &uboHandle);
    //        glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
    //        glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer,
    //                     GL_DYNAMIC_DRAW);
    //
    //        glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uboHandle);

    // Release all
    program_->release();

    nodeDirectionalLight_->setName("Directional Light");
    scene_->getRootNode().addChild(nodeDirectionalLight_);
    nodeDirectionalLight_->getTransform().setPosition(0.0f, 0.0f, 1.0f);

    nodeDirectionalLight_->addComponent(std::make_unique<MoverOnArrows>(1.0f));

    debugWindow_->addSlider(
        "Ambient Strength",
        "Ambient Strength",
        static_cast<int>(ambientStrength * 100), 0, 100,
        [this](int newValue) {
            ambientStrength = static_cast<float>(newValue) / 100.0f;
        });

    debugWindow_->addSlider(
        "Specular Strength",
        "Specular Strength",
        static_cast<int>(specularStrength * 100), 0, 100,
        [this](int newValue) {
            specularStrength = static_cast<float>(newValue) / 100.0f;
        });

    debugWindow_->addSlider(
        "Power Specular",
        "Power Specular",
        powerSpecular, 1, 256,
        [this](int newValue) {
            powerSpecular = newValue;
        });

    debugWindow_->addColorPicker(
        "Light Color",
        "Light Color",
        lightColor,
        [this](QColor newValue) {
            lightColor = std::move(newValue);
        });

    // Еnable depth test and face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Clear all FBO buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    time_.reset();
}

void MorphingView::onRender() {
    const double speedCamera = 1.0f;
    const double speedCameraInShift = 5.0f;

    time_.update();

    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Camera&camera = cameraView_.getCamera();
    camera.move(cameraView_.getDirectionMoveCamera() * static_cast<float>(
                    (cameraView_.isInFastSpeedMode() ? speedCameraInShift : speedCamera) * time_.getDeltaTime()));

    scene_->iterUpdate(time_.getDeltaTime());

    auto viewProjection = camera.getProjectionMatrix() * camera.getViewMatrix();

    // Bind VAO and shader program
    program_->bind();

    //    double r, g, b;
    //    lightColor.getRgbF(&r, &g, &b);
    //    program_->setUniformValue("light_color", r, g, b);
    //    program_->setUniformValue("light_position", nodeDirectionalLight_->getTransform().getPosition());
    //    program_->setUniformValue("view_position", camera.getPosition());

    program_->setUniformValue("ambient_strength", ambientStrength);
    program_->setUniformValue("specular_strength", specularStrength);
    program_->setUniformValue("power_specular", powerSpecular);

    // Draw
    scene_->iterDraw(viewProjection);

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
