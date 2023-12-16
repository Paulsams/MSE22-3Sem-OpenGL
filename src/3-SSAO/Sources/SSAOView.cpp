#include <QFileDialog>
#include "SSAOView.h"
#include "SceneLogic/Camera/Camera.h"
#include "Deferred/Lights/PointLight.h"
#include "LoaderModels/LoaderModel.h"
#include "Utils/InputHandler.h"

using namespace Deferred;

namespace SSAOImpl {
    SSAOView::SSAOView()
            : scene_(std::make_unique<Scene>()),
              debugWindow_(new FieldsDrawer(160)),
              inspector_(new InspectorView(this)),
              sceneView_(new SceneHierarchyView(*inspector_, this)),
              frameCounter_(*debugWindow_, 1000.0f),
              geometryProgram_(std::make_shared<QOpenGLShaderProgram>(this)),
              ssaoProgram_(std::make_shared<QOpenGLShaderProgram>(this)),
              blurProgram_(std::make_shared<QOpenGLShaderProgram>(this)),
              nullForStencilTestProgram_(std::make_shared<QOpenGLShaderProgram>(this)),
              cameraView_(Camera{0.0f, 60.0f, 0.1f, 100.0f}, this),
              gBuffer_(*this) {
        // TODO: весь ниже код существует, потому что не хотелся добавлять бэкграунд почему-то
        auto *parentFormDebugWindow = new QWidget(this);
        debugWindow_->setParent(parentFormDebugWindow);

        parentFormDebugWindow->setStyleSheet("background-color: rgba(122, 122, 122, 200)");

        // TODO: на старом ещё нормально расстягивалось, а тут не хочет
        parentFormDebugWindow->setGeometry(10, 10, 400, 200);
        debugWindow_->setGeometry(5, 5, 390, 190);
        debugWindow_->setFontSize(18);

//        view_position -5.34261, 34.99257, -13.97677    float3

        cameraView_.getCamera().setPosition({0.0f, -35.0f, 14.0f});
        cameraView_.getCamera().rotate(180.0f, 30.0f);

        updateKernels(128);

        connect(&InputHandler::getInstance(), &InputHandler::keyPressed,
            this, [this](int keyCode) {
                keyCode -= Qt::Key_1;
                if (keyCode >= 0 && keyCode <= GBuffer::GBUFFER_NUM_TEXTURES)
                    indexOutputBuffer_ = keyCode;
            });
    }

    void SSAOView::onInit() {
        emptyVAO_.create();

        geometryProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/deferred.vert");
        geometryProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/deferred.frag");

        ssaoProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/ssao.vert");
        ssaoProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/ssao.frag");

        nullForStencilTestProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/null.vert");
        nullForStencilTestProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/null.frag");

        blurProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/blur.vert");
        blurProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/blur.frag");

        // Bind attributes
        geometryProgram_->link();
        geometryProgram_->bind();

//        if (const auto nodeWithLoadModel = LoaderModel::load("../Models/chess.glb", *this,
        if (const auto nodeWithLoadModel = LoaderModel::load("../Models/BrainStem.glb", *this,
                                                             geometryProgram_)) {
            scene_->getRootNode().addChild(nodeWithLoadModel);
            constexpr float scale = 20.0f;
            nodeWithLoadModel->getTransform().setScale(scale, -scale, scale);
        }

        // Release all
        geometryProgram_->release();

        sceneView_->fill(*scene_);

        const auto containerForPointLightsNode = SceneNode::create("Point Lights");
        scene_->getRootNode().addChild(containerForPointLightsNode);

        std::vector<std::pair<const char*, int>> texturesBind = {
                { "g_buffer_position", GBuffer::GBUFFER_TEXTURE_TYPE_POSITION },
                { "g_buffer_color", GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE },
                { "g_buffer_normal", GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL },
                { "g_buffer_ssao", GBuffer::GBUFFER_TEXTURE_TYPE_SSAO_WITH_BLUR },
        };

        points_ = new PointLightsContainer(texturesBind, nullForStencilTestProgram_, this);
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

        directionals_ = new DirectionalLightsContainer(texturesBind, this);
        containerForDirectionalLightsNode->addComponent(std::unique_ptr<DirectionalLightsContainer>(directionals_));

        DirectionalLight& directionalLight = directionals_->addLight();
        directionalLight.setAllData(
                QVector3D(0.4f, 0.4f, 0.4f),
                QVector3D(1.0f, 1.0f, 1.0f),
                QVector3D(0.2f, 0.2f, 0.2f)
        );
        directionalLight.getNode().getTransform().setRotation(QQuaternion::fromAxisAndAngle(
                QVector3D(1.0f, 0.0f, 0.0f),
//                45.0f
                -70.0f
        ));

        debugWindow_->addSlider("Kernel Points", &kernelPointsCount_, 1, kernelPoints_.size());
        debugWindow_->addSpinBox("Sample Radius", &sampleRadiusForSSAO_, 0, 10, 0.05);
        debugWindow_->addSlider("Count Filter Points", &count_points_for_filter, 4, 32);
        debugWindow_->addSpinBox("Stride Filter Points", &stride_points_for_filter, 1.0f, 16.0f, 1.0f);

        time_.reset();
    }

    void SSAOView::onRender() {
        gBuffer_.startFrame();

        // TODO: сделать камеру компонентом и вынести её движение в ещё компонент
        constexpr double speedCamera = 1.0f;
        constexpr double speedCameraInShift = 5.0f;

        time_.update();

        Camera& camera = cameraView_.getCamera();
        camera.move(cameraView_.getDirectionMoveCamera() * static_cast<float>(
                (cameraView_.isInFastSpeedMode() ? speedCameraInShift : speedCamera) * time_.getDeltaTime()));

        scene_->iterUpdate(time_.getDeltaTime());

        const auto viewProjection = camera.getProjectionMatrix() * camera.getViewMatrix();
        auto viewPosition = camera.getPosition();
        // TODO: чёрт знает почему. Надо разбираться.
        // TODO: кешировать юнимформ неймы в индексы
        viewPosition.setX(-viewPosition.x());
        viewPosition.setY(-viewPosition.y());
        viewPosition.setZ(-viewPosition.z());

        geometryPass(viewProjection, viewPosition);
        SSAOPass(camera.getProjectionMatrix());
        blurPass();
        lightsPass(viewProjection, viewPosition);

        auto windowRect = QRect({0, 0}, size());

        gBuffer_.blit(
                nullptr, windowRect,
                windowRect,
                indexOutputBuffer_,
                0, GL_COLOR_BUFFER_BIT, GL_LINEAR
        );

        gBuffer_.getFBO().release();

        frameCounter_.update();

        // Request redraw if animated
        if (animated_)
            update();
    }

    void SSAOView::geometryPass(const QMatrix4x4& viewProjection, const QVector3D& viewPosition) {
        gBuffer_.bindForGeomPass();

        glCullFace(GL_FRONT);
        // Only the geometry pass updates the depth buffer
        glDepthMask(GL_TRUE);

        // Clear buffers
//    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);

        geometryProgram_->link();
        geometryProgram_->bind();

        geometryProgram_->setUniformValue("view_position", viewPosition);

        // Draw
        scene_->iterDraw(cameraView_.getCamera().getViewMatrix(), viewProjection);

        // When we get here the depth buffer is already populated and the stencil pass
        // depends on it, but it does not write to it.
        glDepthMask(GL_FALSE);

        // Release VAO and shader program
        geometryProgram_->release();
    }

    #define ToRadian(x) ((x) * 3.141593f / 180.0f)

    void SSAOView::SSAOPass(const QMatrix4x4& projection) {
        gBuffer_.bindForSSAOPass();
        glCullFace(GL_BACK);
        glDisable(GL_DEPTH_TEST);

        ssaoProgram_->link();
        ssaoProgram_->bind();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBuffer_.getDepthTexture());
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);

        ssaoProgram_->setUniformValue("depth_buffer", 0);
        ssaoProgram_->setUniformValue("sample_radius", sampleRadiusForSSAO_);
        ssaoProgram_->setUniformValue("projection", projection);
        ssaoProgram_->setUniformValue("aspect_ratio", cameraView_.getCamera().getAspect());
        ssaoProgram_->setUniformValue("tan_half_FoV", tanf(ToRadian(cameraView_.getCamera().getFoV() / 2.0f)));
        ssaoProgram_->setUniformValue("kernel_points_count", kernelPointsCount_);
        ssaoProgram_->setUniformValueArray("kernel_points", kernelPoints_.data(), kernelPointsCount_);

        emptyVAO_.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        emptyVAO_.release();

        ssaoProgram_->release();

        glEnable(GL_DEPTH_TEST);
    }

    void SSAOView::blurPass() {
        gBuffer_.bindForBlurPass();

        blurProgram_->link();
        blurProgram_->bind();

        blurProgram_->setUniformValue("g_input_buffer", GBuffer::GBUFFER_TEXTURE_TYPE_SSAO);
        blurProgram_->setUniformValue("count_points", count_points_for_filter);
        blurProgram_->setUniformValue("stride_points", stride_points_for_filter);

        emptyVAO_.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        emptyVAO_.release();

        blurProgram_->release();
    }

    void SSAOView::lightsPass(const QMatrix4x4& viewProjection, const QVector3D& viewPosition) {
        glEnable(GL_STENCIL_TEST);
        points_->executeStencilAndLightPasses(cameraView_.getCamera().getViewMatrix(),
                                              viewProjection, viewPosition, gBuffer_, size());

        // The directional light does not need a stencil test because its volume
        // is unlimited and the final pass simply copies the texture.
        glDisable(GL_STENCIL_TEST);
        directionals_->executeLightPasses(viewPosition, gBuffer_, size());
    }

    void SSAOView::updateKernels(int count) {
        kernelPoints_.clear();
        kernelPoints_.reserve(count);
        kernelPointsCount_ = count / 2;

        for (int i = 0 ; i < count; ++i) {
            float scale = (float)i / (float)(count);
            QVector3D vec;
            vec.setX(2.0f * (float)rand() / RAND_MAX - 1.0f);
            vec.setY(2.0f * (float)rand() / RAND_MAX - 1.0f);
            vec.setZ(2.0f * (float)rand() / RAND_MAX - 1.0f);
            // Use an acceleration function so more points are
            // located closer to the origin
            vec *= (0.1f + 0.9f * scale * scale);

            kernelPoints_.push_back(vec);
        }
    }

    void SSAOView::onResize(size_t width, size_t height) {
        const int widthI = static_cast<int>(width);
        const int heightI = static_cast<int>(height);

        // Configure viewport
        glViewport(0, 0, widthI, heightI);
        gBuffer_.resize(width, height, 0);

        cameraView_.getCamera().setAspect(static_cast<float>(width) / static_cast<float>(height));
        // TODO: хардкод размеров окон для значений
        sceneView_->setGeometry(widthI - 300, 0, 300, 500);
        inspector_->setGeometry(widthI - 450, heightI - 400, 450, 400);
    }
} // SSAOImpl
