#include <QApplication>

#include "DeferredView.h"
#include "Base/Include/Utils/InputHandler.h"

namespace
{
constexpr auto g_sampels = 0;
constexpr auto g_gl_major_version = 3;
constexpr auto g_gl_minor_version = 3;
}// namespace

int main(int argc, char ** argv)
{
	// Create app and set attributes.
	QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
	QApplication app(argc, argv);

	// Set default surface format.
	QSurfaceFormat format;
	format.setSamples(g_sampels);
	format.setVersion(g_gl_major_version, g_gl_minor_version);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

    auto inputHandler = InputHandler::createInstance();

	// Now create window.
	DeferredView window;
	window.resize(1280, 950);
	window.show();

	return app.exec();
}