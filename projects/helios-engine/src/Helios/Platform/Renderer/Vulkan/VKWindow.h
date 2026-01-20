#pragma once

#include "Helios/Engine/Renderer/Window.h"

namespace Helios::Engine::Renderer::Vulkan
{


	class VKWindow : public Window
	{
	public:
		VKWindow(const std::string name);
		~VKWindow();

		// Non-copyable
		VKWindow(const VKWindow&) = delete;
		VKWindow& operator=(const VKWindow&) = delete;

	private:
		void ConfigureWindowHints();
	};


} // namespace Helios::Engine::Renderer::Vulkan
