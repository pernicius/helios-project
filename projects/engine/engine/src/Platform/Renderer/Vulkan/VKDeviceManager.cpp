#include "pch.h"
#include "Platform/Renderer/Vulkan/VKDeviceManager.h"

namespace Helios::Engine::Renderer::Vulkan {


	VKDeviceManager::VKDeviceManager()
	{
		m_Extensions.instance.optional.insert("test_dummy");
		m_Extensions.layer.optional.insert("test_dummy");
		m_Extensions.device.optional.insert("test_dummy");
#if 0
		// setup default required instance extensions
		m_ExtensionsRequired.instance.insert({
			"VK_KHR_surface", // required for any window surface
			});
		// setup default optional instance extensions
		m_ExtensionsOptional.instance.insert({
//			"test_dummy", // just for debugging - will fail!
			});

		// setup default required layers
		m_ExtensionsRequired.layer.insert({
//			"test_dummy", // just for debugging - will fail!
			});
		// setup default optional layers
		m_ExtensionsOptional.layer.insert({
//			"test_dummy", // just for debugging - will fail!
			});

		// setup default required device extensions
		m_ExtensionsRequired.device.insert({
//			"test_dummy", // just for debugging - will fail!
			});
		// setup default optional device extensions
		m_ExtensionsOptional.device.insert({
//			"test_dummy", // just for debugging - will fail!
			// Synchronization
//				"VK_KHR_timeline_semaphore", // easier, cheaper cross-queue sync than binary semaphores
//				"VK_KHR_synchronization2", // better, less error-prone synchronization API and performance improvements
			// Descriptor / resource indexing
//				"VK_EXT_descriptor_indexing", // allow partially-bound descriptors, runtime arrays, non-uniform indexing; enables efficient bindless/GPU-driven rendering
			// GPU address / acceleration
//				"VK_KHR_buffer_device_address", // allow shaders to access buffers via GPU addresses
			// Ray tracing
//				"VK_KHR_ray_tracing_pipeline", // ray tracing pipeline support
//				"VK_KHR_acceleration_structure", // acceleration structure support
//					"VK_KHR_deferred_host_operations", // required by VK_KHR_acceleration_structure
			// Memory & allocation
//				"VK_EXT_memory_budget", // better memory reporting to avoid OOM
//				"VK_EXT_memory_priority", // allow setting memory allocation priority
			// Pipeline/creation helpers
//				"VK_EXT_pipeline_creation_feedback", // diagnose and speed up pipeline creation
//				"VK_EXT_pipeline_creation_cache_control", 
			// Queries & profiling
//				"VK_EXT_host_query_reset", // allows resetting queries on host cheaply
			// Descriptor updates / push
//				"VK_KHR_push_descriptor", // (if frequent small descriptor updates are needed) or use update-after-bind descriptor indexing for bindless
			// Portability & platform-specific
//				"VK_KHR_portability_subset", // (MoltenVK / portability platforms) handle portability constraints
			// Ray queries
//				"VK_KHR_ray_query", // simpler ray tracing without full pipelines/acceleration structures
			// Mesh shaders
//				"VK_EXT_mesh_shader", // modern replacement for vertex+geometry shader stages
			// Fragment shading rate
//				"VK_KHR_fragment_shading_rate", // variable fragment shading rates for performance optimization
			// Others
//				"VK_KHR_device_group", // better support for multi-GPU setups
//				"VK_KHR_parallel_command_execution", // allow command buffers to be recorded while others are executing
			});

		// Features:
		// useful for perf measurement
		//  - pipelineStatisticsQuery
		//  - timestampQuery
#endif

		Init();
	}
	
	
	VKDeviceManager::~VKDeviceManager()
	{
		if (m_Device) {
			m_Device.reset();
		}
		if (m_Instance) {
			m_Instance.reset();
		}
	}


	void VKDeviceManager::Init()
	{
		m_Window = Application::Get().GetAppWindow();

		// create vulkan instance
		m_Instance = CreateRef<VKInstance>(m_Extensions.instance, m_Extensions.layer);
		m_Extensions.instance = m_Instance->GetExtensions();
		m_Extensions.layer = m_Instance->GetLayers();

		// create vulkan window surface
		LOG_RENDER_DEBUG("VKDeviceManager: Creating window surface.");
		vk::SurfaceKHR surface{};
		VkSurfaceKHR cSurf = VK_NULL_HANDLE;
		if (glfwCreateWindowSurface(static_cast<VkInstance>(m_Instance->GetInstance()), m_Window->GetNativeWindow(), nullptr, &cSurf) != VK_SUCCESS) {
			LOG_RENDER_FATAL("VKDeviceManager: Failed to create GLFW window surface.");
			throw std::runtime_error("Failed to create GLFW window surface.");
		}
		surface = static_cast<vk::SurfaceKHR>(cSurf);

		// create vulkan device
		m_Device = CreateRef<VKDevice>(m_Instance, m_Extensions.device, surface);
		m_Extensions.device = m_Device->GetExtensions();
	}


} // namespace Helios::Engine::Renderer::Vulkan
