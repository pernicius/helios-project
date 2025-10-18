#pragma once

#include "Helios/Engine/Renderer/Window.h"

namespace Helios::Engine
{


	class GLWindow : public Window
	{
	public:
		GLWindow() {};
		~GLWindow() = default;

		virtual void OnUpdate() override {}
	};


}// namespace Helios::Engine
