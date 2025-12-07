#include "pch.h"
#include "Helios/Engine/Spec/SpecDevice.h"

namespace Helios::Engine::Spec {


	// default values
//	Renderer::Format Device::swapChainFormat = Renderer::Format::SRGBA8_UNORM; // Default to a commonly supported format with sRGB
	Renderer::Format Device::swapChainFormat = Renderer::Format::SBGRA8_UNORM; // used in nvrhi-donut DeviceManager_VK.cpp
	int              Device::swapChainSampleCount = 1;


} // namespace Helios::Engine::Spec
