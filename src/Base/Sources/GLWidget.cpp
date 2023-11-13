#include "GLWidget.hpp"
#include "Utils/InputHandler.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

InputHandler *InputHandler::_instance = nullptr;

namespace fgl {

    GLWidget::ContextGuard::ContextGuard(GLWidget &self)
            : self_{self} {
        self_.makeCurrent();
    }

    GLWidget::ContextGuard::~ContextGuard() {
        self_.doneCurrent();
    }

    auto GLWidget::bindContext() noexcept -> ContextGuard {
        return ContextGuard{*this};
    }

    void GLWidget::initializeGL() {
        initializeOpenGLFunctions();

        {
            const auto guard = bindContext();
            onInit();
        }
    }

    void GLWidget::resizeGL(const int width, const int height) {
        const auto retinaScale = devicePixelRatio();
        onResize(static_cast<size_t>(width * retinaScale),
                 static_cast<size_t>((height ? height : 1) * retinaScale));
    }

    void GLWidget::paintGL() {
        onRender();
    }

}// namespace fgl
