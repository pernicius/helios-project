#pragma once

#include "Helios/Engine/Renderer/Window.h"

namespace Helios::Engine::Renderer
{


	class VKWindow : public Window
	{
	public:
		VKWindow();
		~VKWindow();

		virtual void OnUpdate() override;

		virtual void Show() override;
		virtual void Hide() override;
	};


} // namespace Helios::Engine::Renderer
