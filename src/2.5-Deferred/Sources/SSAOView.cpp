#include "SSAOView.h"

#include "Base/Include/SceneLogic/Scene.h"
#include "Base/Include/SceneLogic/SceneNode.h"
#include "Base/Include/LoaderModels/LoaderModel.h"
#include "Base/Include/Views/FieldsDrawer.h"
#include "Base/Include/Utils/ToggleSwitch.h"
#include "Base/Include/Utils/InputHandler.h"

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include <memory>

SSAOView::SSAOView()
    : scene_(std::make_unique<Scene>()),
      debugWindow_(new FieldsDrawer(160)),
      inspector_(new InspectorView(this)),
      sceneView_(new SceneHierarchyView(*inspector_, this)),
      frameCounter_(*debugWindow_, 1000.0f),
      geometryProgram_(std::make_shared<QOpenGLShaderProgram>(this)),
      nullForStencilTestProgram_(std::make_shared<QOpenGLShaderProgram>(this)),
      cameraView_(Camera{0.0f, 60.0f, 0.1f, 100.0f}, this),
      gBuffer_(*this) {
    QGridLayout grid;

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

    connect(&InputHandler::getInstance(), &InputHandler::keyPressed,
            this, [this](int keyCode) {
        if (keyCode == Qt::Key_G)
            isNeedChangeSamples_ = true;
    });
}

void SSAOView::onInit() {
    geometryProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/deferred.vert");
    geometryProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/deferred.frag");

    nullForStencilTestProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/null.vert");
    nullForStencilTestProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/null.frag");

    // Bind attributes
    geometryProgram_->link();
    geometryProgram_->bind();

    // if (const auto nodeWithLoadModel = LoaderModel::load("../Models/Two Models.glb", *this, program_)) {
    if (const auto nodeWithLoadModel = LoaderModel::load("../Models/chess.glb", *this,
                                                         geometryProgram_)) {
        scene_->getRootNode().addChild(nodeWithLoadModel);
    }

    // Release all
    geometryProgram_->release();

    debugWindow_->addColorPicker("Clear color", &clearColor_);

    sceneView_->fill(*scene_);

    const auto containerForPointLightsNode = SceneNode::create("Point Lights");
    scene_->getRootNode().addChild(containerForPointLightsNode);

    points_ = new PointLightsContainer(nullForStencilTestProgram_, this);
    containerForPointLightsNode->addComponent(std::unique_ptr<PointLightsContainer>(points_));

    PointLight& firstPoint = points_->addLight();
    firstPoint.setAllData(
            10.0f,
            QVector3D(0.0f, 0.0f, 0.0f),
            QVector3D(1.0f, 1.0f, 1.0f),
            QVector3D(1.0f, 1.0f, 1.0f),
            1.0f,
            0.4f,
            0.6f
    );
    firstPoint.getNode().getTransform().setPosition(0.0f, 6.0f, 0.0f);

    const auto containerForDirectionalLightsNode = SceneNode::create("Directional Lights");
    scene_->getRootNode().addChild(containerForDirectionalLightsNode);

    directionals_ = new DirectionalLightsContainer(this);
    containerForDirectionalLightsNode->addComponent(std::unique_ptr<DirectionalLightsContainer>(directionals_));

    DirectionalLight& directionalLight = directionals_->addLight();
    directionalLight.setAllData(
            QVector3D(0.7f, 0.7f, 0.7f),
            QVector3D(0.1f, 0.6f, 0.9f),
            QVector3D(1.0f, 1.0f, 1.0f)
    );
    directionalLight.getNode().getTransform().setRotation(QQuaternion::fromAxisAndAngle(
            QVector3D(1.0f, 0.0f, 0.0f),
            45.0f
    ));

    // Clear all FBO buffers
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    debugWindow_->addButton("Load Model", "Open Explorer", [this]() {
    	QString filePath = QFileDialog::getOpenFileName(this);
        if (filePath.isEmpty())
            return;

    	if (const auto nodeWithLoadModel = LoaderModel::load(filePath.toLocal8Bit().data(), *this, geometryProgram_)) {
			scene_->getRootNode().addChild(nodeWithLoadModel);
		}
    });

    time_.reset();
}

void SSAOView::onRender() {
    if (isNeedChangeSamples_) {
        countSamplesFromGBuffer_ = countSamplesFromGBuffer_ == 0 ? 16 : 0;
        gBuffer_.resize(size().width(), size().height(), countSamplesFromGBuffer_);
        isNeedChangeSamples_ = false;
    }

    gBuffer_.startFrame();

	gBuffer_.bindForGeomPass();

    // TODO: сделать камеру компонентом и вынести её движение в ещё компонент
    constexpr double speedCamera = 1.0f;
    constexpr double speedCameraInShift = 5.0f;

    // Only the geometry pass updates the depth buffer
    glDepthMask(GL_TRUE);

    // Clear buffers
//    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    time_.update();

    Camera& camera = cameraView_.getCamera();
    camera.move(cameraView_.getDirectionMoveCamera() * static_cast<float>(
                    (cameraView_.isInFastSpeedMode() ? speedCameraInShift : speedCamera) * time_.getDeltaTime()));

    scene_->iterUpdate(time_.getDeltaTime());

    const auto viewProjection = camera.getProjectionMatrix() * camera.getViewMatrix();

    // Bind VAO and shader program
	geometryProgram_->link();
    geometryProgram_->bind();

    // TODO: чёрт знает почему. Надо разбираться.
    auto viewPosition = camera.getPosition();
    viewPosition.setX(-viewPosition.x());
    viewPosition.setY(-viewPosition.y());
    viewPosition.setZ(-viewPosition.z());
    // TODO: кешировать юнимформ неймы в индексы
    geometryProgram_->setUniformValue("view_position", viewPosition);

    // Draw
    scene_->iterDraw(viewProjection);

    // When we get here the depth buffer is already populated and the stencil pass
    // depends on it, but it does not write to it.
    glDepthMask(GL_FALSE);

    // Release VAO and shader program
    geometryProgram_->release();

//    glClearColor(clearColor_.redF(), clearColor_.greenF(), clearColor_.blueF(), clearColor_.alphaF());
//    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_STENCIL_TEST);
    points_->executeStencilAndLightPasses(viewProjection, viewPosition, gBuffer_, size());

    // The directional light does not need a stencil test because its volume
    // is unlimited and the final pass simply copies the texture.
    glDisable(GL_STENCIL_TEST);
    directionals_->executeLightPasses( viewPosition, gBuffer_, size());

    auto windowRect = QRect({0, 0}, size());
    gBuffer_.blit(
        nullptr, windowRect,
        windowRect,
        3,
        0, GL_COLOR_BUFFER_BIT, GL_LINEAR
    );

    gBuffer_.getFBO().release();

    frameCounter_.update();

    // Request redraw if animated
    if (animated_)
        update();
}

void SSAOView::onResize(const size_t width, const size_t height) {
	const int widthI = static_cast<int>(width);
	const int heightI = static_cast<int>(height);

    // Configure viewport
    glViewport(0, 0, widthI, heightI);
	gBuffer_.resize(width, height, countSamplesFromGBuffer_);

    cameraView_.getCamera().setAspect(static_cast<float>(width) / static_cast<float>(height));
    // TODO: хардкод размеров окон для значений
    sceneView_->setGeometry(widthI - 300, 0, 300, 500);
    inspector_->setGeometry(widthI - 450, heightI - 400, 450, 400);
}
