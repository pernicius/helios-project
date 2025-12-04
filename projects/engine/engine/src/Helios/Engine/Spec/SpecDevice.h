#pragma once

#include "Helios/Engine/Renderer/Format.h"

namespace Helios::Engine::Spec {


	class Device
	{
	public:
		static Renderer::Format swapChainFormat;
		static int              swapChainSampleCount;
	};


} // namespace Helios::Engine::Spec
