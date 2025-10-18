#pragma once

#include "Helios/Engine/Renderer/Window.h"

namespace Helios::Engine
{


	class VKWindow : public Window
	{
	public:
		VKWindow() {};
		~VKWindow() = default;

		virtual void OnUpdate() override {}
	};


}// namespace Helios::Engine
