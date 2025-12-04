#include "pch.h"
#include "Helios/Engine/Spec/SpecWindow.h"

namespace Helios::Engine::Spec {


	// default values
	bool        Window::enablePerMonitorDPI          = false;
	bool        Window::resizeWindowWithDisplayScale = false;
	bool        Window::allowResizing                = true;

	bool        Window::startBorderless              = false;
	bool        Window::startFullscreen              = false;
	bool        Window::startMaximized               = false;

	int         Window::posX                         = -1;
	int         Window::posY                         = -1;
	int         Window::sizeX                        = 800;
	int         Window::sizeY                        = 600;

	int         Window::refreshRate                  = 60;

	std::string Window::windowTitle                  = "DummyTitle";


} // namespace Helios::Engine::Spec
