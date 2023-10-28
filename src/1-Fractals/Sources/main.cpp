#include <QApplication>
#include <QSurfaceFormat>

#include "FractalView.h"
#include "FractalRenderers/Mandelbrot.h"
#include "MoveFractalBehaviours/DragBehaviour.h"

namespace {
    constexpr auto g_sampels = 16;
    constexpr auto g_gl_major_version = 3;
    constexpr auto g_gl_minor_version = 3;
}// namespace

int main(int argc, char **argv) {
    // Create app and set attributes.
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);

    // Set default surface format.
    QSurfaceFormat format;
    format.setSamples(g_sampels);
    format.setVersion(g_gl_major_version, g_gl_minor_version);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    // Now create window.
    Fractals::FractalView window(std::make_unique<Fractals::Mandelbrot>(400.0f),
                                 std::make_unique<Fractals::DragBehaviour>());
    window.resize(640, 480);
    window.show();

    return app.exec();
}
