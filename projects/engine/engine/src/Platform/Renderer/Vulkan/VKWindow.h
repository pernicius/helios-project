#pragma once

#include "Helios/Engine/Renderer/Window.h"

namespace Helios::Engine::Renderer::Vulkan
{


	class VKWindow : public Window
	{
	public:
		VKWindow();
		~VKWindow();

		// Non-copyable
		VKWindow(const VKWindow&) = delete;
		VKWindow& operator=(const VKWindow&) = delete;

	public:
		virtual void OnUpdate() override;

		virtual void Show() override;
		virtual void Hide() override;
	};


} // namespace Helios::Engine::Renderer::Vulkan
