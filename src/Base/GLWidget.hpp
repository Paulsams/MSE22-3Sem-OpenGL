#pragma once

#include <QOpenGLExtraFunctions>
#include <QOpenGLWidget>

namespace fgl {

    class GLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT

    public:
        using QOpenGLWidget::QOpenGLWidget;

        ~GLWidget() override = default;

    public:
        virtual void onInit() = 0;

        virtual void onRender() = 0;

        virtual void onResize(size_t width, size_t height) = 0;

    public:
        class ContextGuard final {
        public:
            explicit ContextGuard(GLWidget &self);

            ~ContextGuard();

            ContextGuard(const ContextGuard &) = delete;

            ContextGuard(ContextGuard &&) = delete;

            ContextGuard &operator=(const ContextGuard &) = delete;

            ContextGuard &operator=(ContextGuard &&) = delete;

        private:
            GLWidget &self_;
        };

        [[nodiscard]] ContextGuard bindContext() noexcept;

    private:// QOpenGLWidget
        void initializeGL() override;

        void resizeGL(int width, int height) override;

        void paintGL() override;
    };

}// namespace fgl
