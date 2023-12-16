#include "MorphingView.h"
#include "Base/Include/SceneLogic/Scene.h"
#include "Base/Include/LoaderModels/LoaderModel.h"
#include "Base/Include/Views/FieldsDrawer.h"
#include "Base/Include/Utils/ToggleSwitch.h"
#include "Lights/LightsContainer.h"
#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"
#include "Lights/SpotLight.h"

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include <memory>

MorphingView::MorphingView() noexcept
    : scene_(std::make_unique<Scene>()),
      debugWindow_(new FieldsDrawer(160)),
      inspector_(new InspectorView(this)),
      sceneView_(new SceneHierarchyView(*inspector_, this)),
      frameCounter_(*debugWindow_, 1000.0f),
      program_(std::make_shared<QOpenGLShaderProgram>(this)),
      cameraView_(Camera{0.0f, 60.0f, 0.1f, 100.0f}, this) {

    // TODO: весь ниже код существует, потому что не хотелся добавлять бэкграунд почему-то
    auto* parentFormDebugWindow = new QWidget(this);
    debugWindow_->setParent(parentFormDebugWindow);

    parentFormDebugWindow->setStyleSheet("background-color: rgba(122, 122, 122, 200)");

    // TODO: на старом ещё нормально расстягивалось, а тут не хочет
    parentFormDebugWindow->setGeometry(10, 10, 400, 200);
    debugWindow_->setGeometry(5, 5, 390, 190);
    debugWindow_->setFontSize(18);

    cameraView_.getCamera().setPosition({0.0f, -10.0f, 0.0f});
    cameraView_.getCamera().rotate(0.0f, 30.0f);
}

MorphingView::~MorphingView() {
    {
        // Free resources with context bounded.
        const auto guard = bindContext();
        program_.reset();
    }
}

void MorphingView::onInit() {
    // Configure shaders
    program_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/diffuse.vert");
    program_->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                      ":/Shaders/diffuse.frag");
    program_->link();

    // Bind attributes
    program_->bind();

    // if (const auto nodeWithLoadModel = LoaderModel::load("../Models/Two Models.glb", *this, program_)) {
    if (const auto nodeWithLoadModel = LoaderModel::load("../Models/chess.glb", *this, program_)) {
        scene_->getRootNode().addChild(nodeWithLoadModel);
    }

    createDirectionalLight();
    createPointLights();
    createSpotLights();

    // Release all
    program_->release();

    debugWindow_->addSlider("Power Specular", &powerSpecular_, 1, 256);
    debugWindow_->addColorPicker("Clear color", &clearColor_);
    debugWindow_->addToggle("Is Morphing", &isMorphing_);

    sceneView_->fill(*scene_);

    // Еnable depth test and face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Clear all FBO buffers
    glClearColor(clearColor_.redF(), clearColor_.greenF(), clearColor_.blueF(), clearColor_.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    time_.reset();
}

void MorphingView::onRender() {
    constexpr double speedCamera = 1.0f;
    constexpr double speedCameraInShift = 5.0f;

    time_.update();

    // Clear buffers
    glClearColor(clearColor_.redF(), clearColor_.greenF(), clearColor_.blueF(), clearColor_.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Camera& camera = cameraView_.getCamera();
    camera.move(cameraView_.getDirectionMoveCamera() * static_cast<float>(
                    (cameraView_.isInFastSpeedMode() ? speedCameraInShift : speedCamera) * time_.getDeltaTime()));

    scene_->iterUpdate(time_.getDeltaTime());

    const auto viewProjection = camera.getProjectionMatrix() * camera.getViewMatrix();

    // Bind VAO and shader program
    program_->bind();
    for (ILightsContainer* lightsContainer : lightsContainers_)
        lightsContainer->bindLights();

    program_->setUniformValue("power_specular", powerSpecular_);
    // TODO: чёрт знает почему. Надо разбираться.
    auto viewPosition = camera.getPosition();
    viewPosition.setX(-viewPosition.x());
    viewPosition.setY(-viewPosition.y());
    viewPosition.setZ(-viewPosition.z());
    // TODO: кешировать юнимформ неймы в индексы
    program_->setUniformValue("view_position", viewPosition);
    program_->setUniformValue("time", time_.getTime());
    program_->setUniformValue("radius", 1.0f);
    program_->setUniformValue("blend", std::cos(time_.getTime()) / 2.0f + 0.5f);
    program_->setUniformValue("is_morphing", isMorphing_);

    // Draw
    scene_->iterDraw(cameraView_.getCamera().getViewMatrix(), viewProjection);

    // Release VAO and shader program
    program_->release();

    frameCounter_.update();

    // Request redraw if animated
    if (animated_) {
        update();
    }
}

void MorphingView::createDirectionalLight() {
    const auto containerForDirectionalLightsNode = SceneNode::create("Directional Lights");
    const auto lightsContainer = new LightsContainer<DirectionalLight>(
        *this, 0, 1,
        *program_, "DirectionalLight"
    );
    containerForDirectionalLightsNode->addComponent(std::unique_ptr<LightsContainer<DirectionalLight>>(lightsContainer));

    DirectionalLight& light = lightsContainer->addLight();
    light.getNode().getTransform().setRotation(QQuaternion::fromAxisAndAngle(
        QVector3D(1.0f, 0.0f, 0.0f),
        45.0f
    ));
    light.setAllData(
        {0.1f, 0.1f, 0.1f},
        0.05f
    );
    light.setColor({0.1f, 0.1f, 0.1f});
    light.setAmbientStrength(0.05f);

    lightsContainers_.push_back(lightsContainer);
    scene_->getRootNode().addChild(containerForDirectionalLightsNode);
}

void MorphingView::createPointLights() {
    const auto containerForPointLightsNode = SceneNode::create("Point Lights");
    const auto lightsContainer = new LightsContainer<PointLight>(
        *this, 1, 10,
        *program_, "PointLights"
    );
    containerForPointLightsNode->addComponent(std::unique_ptr<LightsContainer<PointLight>>(lightsContainer));

    PointLight& firstPoint = lightsContainer->addLight();
    firstPoint.getNode().getTransform().setPosition({0.0f, 7.0f, -10.0f});
    firstPoint.setAllData(
        10.0f,
        // TODO: а нужен ли вообще эмбиент, кроме направленного источника???
        {0.0f, 0.0f, 0.0f},
        {0.5f, 1.0f, 0.5f},
        {0.5f, 1.0f, 0.5f},
        1.0f,
        0.4f,
        0.6f
    );

    if (const auto nodeWithLoadModel = LoaderModel::load("../Models/Sphere.glb", *this, program_)) {
        firstPoint.getNode().addChild(nodeWithLoadModel);
    }

    PointLight& secondPoint = lightsContainer->addLight();
    secondPoint.getNode().getTransform().setPosition({0.0f, 7.0f, 10.0f});
    secondPoint.setAllData(
        5.0f,
        {0.0f, 0.0f, 0.0f},
        {1.0f, 0.5f, 0.8f},
        {1.0f, 0.5f, 0.8f},
        1.0f,
        0.9f,
        2.0f
    );

    if (const auto nodeWithLoadModel = LoaderModel::load("../Models/Sphere.glb", *this, program_)) {
        secondPoint.getNode().addChild(nodeWithLoadModel);
    }

    lightsContainers_.push_back(lightsContainer);
    scene_->getRootNode().addChild(containerForPointLightsNode);
}

void MorphingView::createSpotLights() {
    const auto containerForPointLightsNode = SceneNode::create("Spot Lights");
    const auto lightsContainer = new LightsContainer<SpotLight>(
        *this, 2, 10,
        *program_, "SpotLights"
    );
    containerForPointLightsNode->addComponent(std::unique_ptr<LightsContainer<SpotLight>>(lightsContainer));

    SpotLight& firstLight = lightsContainer->addLight();
    firstLight.getNode().getTransform().setPosition({10.0f, 7.0f, 0.0f});
    firstLight.getNode().getTransform().setRotation(QQuaternion::fromAxisAndAngle(
        QVector3D(1.0f, 0.0f, 0.0f),
        180.0f
    ));

    firstLight.setAllData(
        5.0f,
        30.0f,
        50.0f,
        {0.0f, 0.0f, 0.0f},
        {0.7f, 0.3f, 0.1f},
        {0.1f, 0.4f, 0.8f}
    );

    SpotLight& secondLight = lightsContainer->addLight();
    secondLight.getNode().getTransform().setPosition({-10.0f, 7.0f, 0.0f});
    secondLight.getNode().getTransform().setRotation(QQuaternion::fromEulerAngles(
        0.0f, 180.0f, 150.0f
    ));

    secondLight.setAllData(
        13.0f,
        60.0f,
        80.0f,
        {0.0f, 0.0f, 0.0f},
        {0.9f, 0.1f, 0.7f},
        {0.2f, 0.2f, 0.2f}
    );

    lightsContainers_.push_back(lightsContainer);
    scene_->getRootNode().addChild(containerForPointLightsNode);
}

void MorphingView::onResize(const size_t width, const size_t height) {
    // Configure viewport
    glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));

    cameraView_.getCamera().setAspect(static_cast<float>(width) / static_cast<float>(height));
    // TODO: хардкод размеров окон для значений
    sceneView_->setGeometry(static_cast<int>(width) - 300, 0, 300, 500);
    inspector_->setGeometry(static_cast<int>(width) - 450, static_cast<int>(height) - 400, 450, 400);
}
