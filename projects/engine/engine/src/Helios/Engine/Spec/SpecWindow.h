#pragma once

namespace Helios::Engine::Spec {


	class Window
	{
	public:
		static bool enablePerMonitorDPI;
		static bool resizeWindowWithDisplayScale;
		static bool allowResizing;

		static bool startBorderless;
		static bool startFullscreen;
		static bool startMaximized;

		static int posX;
		static int posY;
		static int sizeX;
		static int sizeY;
		static int refreshRate;

		static std::string windowTitle;
	};


} // namespace Helios::Engine::Spec
