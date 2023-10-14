#include <QApplication>
#include <QSurfaceFormat>

#include "FractalView.h"

namespace
{
constexpr auto g_sampels = 16;
constexpr auto g_gl_major_version = 3;
constexpr auto g_gl_minor_version = 3;
constexpr auto g_depthBufferSize = 32;
}// namespace

int main(int argc, char ** argv)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(g_depthBufferSize);
    format.setSamples(g_sampels);
    format.setVersion(g_gl_major_version, g_gl_minor_version);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication app(argc, argv);

    Fractals::FractalView window;
    window.resize(640, 480);
	window.show();

	return app.exec();
}