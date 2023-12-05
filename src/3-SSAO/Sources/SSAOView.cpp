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
      pointLightsProgram_(std::make_shared<QOpenGLShaderProgram>(this)),
      spotLightsProgram_(std::make_shared<QOpenGLShaderProgram>(this)),
      direcionalLightsProgram_(std::make_shared<QOpenGLShaderProgram>(this)),
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
    blitter_.create();

    geometryProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/deferred.vert");
    geometryProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/deferred.frag");
    geometryProgram_->link();

	pointLightsProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/light.vert");
	pointLightsProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/PointLight.frag");

    // Bind attributes
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

    // Еnable depth test and face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_MULTISAMPLE);

    // Clear all FBO buffers
    glClearColor(clearColor_.redF(), clearColor_.greenF(), clearColor_.blueF(), clearColor_.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    debugWindow_->addButton("Load Model", "Open Explorer", [this]() {
    	QString filePath = QFileDialog::getOpenFileName(this);
        if (filePath.isEmpty())
            return;

    	if (const auto nodeWithLoadModel = LoaderModel::load(filePath.toLocal8Bit().data(), *this, geometryProgram_)) {
			scene_->getRootNode().addChild(nodeWithLoadModel);
		}
    });

	pointLightsProgram_->link();
	_shpereForPointLight = LoaderModel::load("../Models/Sphere.glb", *this, pointLightsProgram_);

    time_.reset();
}

void SSAOView::onRender() {
    if (isNeedChangeSamples_) {
        countSamplesFromGBuffer_ = countSamplesFromGBuffer_ == 0 ? 16 : 0;
        gBuffer_.resize(size().width(), size().height(), countSamplesFromGBuffer_);
        isNeedChangeSamples_ = false;
    }

	gBuffer_.bindForWriting();

    // TODO: сделать камеру компонентом и вынести её движение в ещё компонент
    constexpr double speedCamera = 1.0f;
    constexpr double speedCameraInShift = 5.0f;

    // Clear buffers
    glClearColor(clearColor_.redF(), clearColor_.greenF(), clearColor_.blueF(), clearColor_.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    // Release VAO and shader program
    geometryProgram_->release();
	gBuffer_.release();

    gBuffer_.bindForReading();
    auto windowRect = QRect({0, 0}, size());

    // TODO: лучше хотя бы бульку, что ли?
    if (countSamplesFromGBuffer_ == 0) {

        int halfWidth = windowRect.size().width() / 2;
        int halfHeight = windowRect.size().height() / 2;
        auto rect = QRect({0, 0}, QSize(halfWidth, halfHeight));

        gBuffer_.blit(nullptr, rect,
                      windowRect,
                      GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);

        gBuffer_.blit(nullptr, rect.translated(halfWidth, 0),
                      windowRect,
                      GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);

        gBuffer_.blit(nullptr, rect.translated(0, halfHeight),
                      windowRect,
                      GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);

        gBuffer_.blit(nullptr, rect.translated(halfWidth, halfHeight),
                      windowRect,
                      GBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD);
    } else {
        gBuffer_.blit(nullptr, windowRect, windowRect, GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    }

    gBuffer_.release();

	// glEnable(GL_BLEND);
	// glBlendEquation(GL_FUNC_ADD);
	// glBlendFunc(GL_ONE, GL_ONE);
	//
	// pointLightsPass(viewProjection);

	// glClear(GL_COLOR_BUFFER_BIT);

    frameCounter_.update();

    // Request redraw if animated
    if (animated_)
        update();
}

void SSAOView::pointLightsPass(const QMatrix4x4& viewProjection) {
	pointLightsProgram_->link();
	pointLightsProgram_->bind();

	pointLightsProgram_->setUniformValue("screen_size", size());
	// auto textures = gBuffer->textures();
	// glActiveTexture(GL_TEXTURE0 + 0);
	// glBindTexture(GL_TEXTURE_2D, textures[0]);
	// glActiveTexture(GL_TEXTURE0 + 1);
	// glBindTexture(GL_TEXTURE_2D, textures[1]);

	auto modelMatrix = _shpereForPointLight->getTransform().getModelMatrix();
	modelMatrix.scale(5.0f, 5.0f, 5.0f);

	// TODO: костыль -- подумать, что может сделать сферу дефолтной моделью
	_shpereForPointLight->getChildren()[0]->getChildren()[0]->getChildren()[0]->getRenderer()->draw(modelMatrix, viewProjection);

	pointLightsProgram_->release();
}

void SSAOView::onResize(const size_t width, const size_t height) {
	const int witdhI = static_cast<int>(width);
	const int heightI = static_cast<int>(height);

    // Configure viewport
    glViewport(0, 0, witdhI, heightI);
	gBuffer_.resize(width, height, countSamplesFromGBuffer_);

    cameraView_.getCamera().setAspect(static_cast<float>(width) / static_cast<float>(height));
    // TODO: хардкод размеров окон для значений
    sceneView_->setGeometry(witdhI - 300, 0, 300, 500);
    inspector_->setGeometry(witdhI - 450, heightI - 400, 450, 400);
}
