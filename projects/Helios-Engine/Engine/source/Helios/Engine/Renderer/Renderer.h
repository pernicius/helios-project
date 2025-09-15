#pragma once

//#include "Helios/Engine/Renderer/RendererSpec.h"

namespace Helios::Engine {


	// Renderer and Window related configuration
	namespace Renderer {
		struct Specification
		{
			struct WindowSpec
			{
				std::string windowTitle = "HeliosEngine - Default Title";

				// Enables per-monitor DPI scale support.
				//
				// If set to true, the app will receive DisplayScaleChanged() events on DPI change and can read
				// the scaling factors using GetDPIScaleInfo(...). The window may be resized when DPI changes if
				// DeviceCreationParameters::resizeWindowWithDisplayScale is true.
				//
				// If set to false, the app will see DPI scaling factors being 1.0 all the time, but the OS
				// may scale the contents of the window based on DPI.
				bool enablePerMonitorDPI = false;

				bool startMaximized = false; // ignores backbuffer width/height to be monitor size
				bool startFullscreen = false;
				bool startBorderless = false;

				bool allowResizing = true;
//				bool allowModeSwitch = false;

				int posX = -1; // -1 means use default placement
				int posY = -1; // -1 means use default placement

				// Enables automatic resizing of the application window according to the DPI scaling of the monitor
				// that it is located on. When set to true and the app launches on a monitor with >100% scale, 
				// the initial window size will be larger than specified in 'backBufferWidth' and 'backBufferHeight' parameters.
				bool resizeWindowWithDisplayScale = false;
			} Window;

			struct InstanceSpec
			{
//				bool enableDebugRuntime = false;
//				bool enableWarningsAsErrors = false;
//				bool enableGPUValidation = false; // Affects only DX12
//				bool logBufferLifetime = false;
//				bool enableHeapDirectlyIndexed = false; // Allows ResourceDescriptorHeap on DX12

#				ifdef HE_RENDERER_VULKAN
//					std::vector<std::string> requiredVulkanExtensions;
//					std::vector<std::string> requiredVulkanLayers;
//					std::vector<std::string> optionalVulkanExtensions;
//					std::vector<std::string> optionalVulkanLayers;
#				endif
			} Instance;


			struct DeviceSpec
			{
//				bool headlessDevice = false;

				uint32_t backBufferWidth = 1280;
				uint32_t backBufferHeight = 720;
				uint32_t refreshRate = 0;

//				uint32_t swapChainBufferCount = 3;
				nvrhi::Format swapChainFormat = nvrhi::Format::SRGBA8_UNORM;
				uint32_t swapChainSampleCount = 1;
//				uint32_t swapChainSampleQuality = 0;

//				uint32_t maxFramesInFlight = 2;
//				bool enableNvrhiValidationLayer = false;
//				bool vsyncEnabled = false;
//				bool enableRayTracingExtensions = false; // for vulkan
//				bool enableComputeQueue = false;
//				bool enableCopyQueue = false;

				// Index of the adapter (DX11, DX12) or physical device (Vk) on which to initialize the device.
				// Negative values mean automatic detection.
				// The order of indices matches that returned by DeviceManager::EnumerateAdapters.
//				int adapterIndex = -1;

#				if HE_RENDERER_DX11 || HE_RENDERER_DX12
//					DXGI_USAGE swapChainUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
//					D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
#				endif

#				if HE_RENDERER_VULKAN
//					std::vector<std::string> requiredVulkanExtensions;
//					std::vector<std::string> optionalVulkanExtensions;
//					std::vector<size_t> ignoredVulkanValidationMessageLocations = {
						// Ignore the warnings like "the storage image descriptor [...] is accessed by a OpTypeImage that has
						// a Format operand ... which doesn't match the VkImageView ..." -- even when the GPU supports
						// storage without format, which all modern GPUs do, there is no good way to enable it in the shaders.
//						0x13365b2
//					};
//					std::function<void(VkDeviceCreateInfo&)> deviceCreateInfoCallback;

					// This pointer specifies an optional structure to be put at the end of the chain for 'vkGetPhysicalDeviceFeatures2' call.
					// The structure may also be a chain, and must be alive during the device initialization process.
					// The elements of this structure will be populated before 'deviceCreateInfoCallback' is called,
					// thereby allowing applications to determine if certain features may be enabled on the device.
//					void* physicalDeviceFeatures2Extensions = nullptr;
#				endif
			} Device;
		};


	} // end of namespace Helios::Engine::Renderer
} // end of namespace Helios::Engine
