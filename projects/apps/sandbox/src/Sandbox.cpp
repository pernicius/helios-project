#include "pch.h"
namespace HE = Helios::Engine;

//#include "Platform/Renderer/Vulkan/VKDeviceManager.h"
//#include "Platform/Renderer/Vulkan/VKRenderPass.h"
//#include "Platform/Renderer/Vulkan/VKPipeline.h"

#include "Helios/Engine/VFS/VFS.h"

class App : public HE::Application
{
public:
	App(const HE::AppSpec& spec) : HE::Application(spec) {}

	static constexpr int APP_VERSION_MAJOR = 0;
	static constexpr int APP_VERSION_MINOR = 1;
	static constexpr int APP_VERSION_PATCH = 0;
	static constexpr uint32_t APP_VERSION = HE_MAKE_VERSION(APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH);

	virtual void OnInit() override;
	virtual void OnShutdown() override;

private:
	// simple renderpass + pipeline handles owned by the app
//	Helios::Ref<HE::Renderer::Vulkan::VKRenderPass> m_RenderPass;
//	Helios::Ref<HE::Renderer::Vulkan::VKPipeline> m_GraphicsPipeline;
};


HE::Application* HE::CreateApplication(int argc, char** argv)
{
	// Init App Specification
	HE::AppSpec appSpec;
	appSpec.CmdLineArgs = { argc, argv };
	appSpec.Name = "Sandbox";
	appSpec.Version = App::APP_VERSION;
	appSpec.Hints |= HE::AppSpec::HintFlags::USE_EXEPATH;
	appSpec.LogFile = "Sandbox.log";
	appSpec.ConfigFile = "Sandbox.cfg";

	return new App(appSpec);
}


void App::OnInit()
{
	LOG_INFO("Sandbox: Init.");

	InitRenderer();

	VirtFS.Mount("assets", ".");
	VirtFS.CreateAlias("@assets:", "assets/");
	VirtFS.CreateAlias("@textures:", "assets/textures/");

	// Create a simple render pass + pipeline now that renderer (window/device/swapchain) exists.
//	try {
//		LOG_DEBUG("Sandbox: Creating simple render pass and graphics pipeline.");
//		using namespace Helios::Engine::Renderer::Vulkan;
//
//		auto& dm = VKDeviceManager::Get();
//		auto deviceRef = dm.GetDevice();
//		auto device = deviceRef->GetDevice();
//
//		// Query swapchain format via device manager
//		auto swap = dm.GetSwapchain();
//		vk::Format swapFormat = vk::Format::eB8G8R8A8Unorm;
//		if (swap) swapFormat = swap->GetFormat();
//
//		// --- create simple color-only render pass using builder ---
//		VKRenderPassBuilder rpb(deviceRef);
//		m_RenderPass = rpb.BuildSimple(deviceRef, swapFormat);
//
//		// --- build a simple graphics pipeline using test shaders ---
//		// Ensure SPV files exist at runtime (premake places them in build output under assets/shader/vulkan/)
//		const std::string shaderBase = "../helios-engine/assets/shader/vulkan/";
//		VKPipelineBuilder pb(deviceRef);
//		pb.AddShaderStageFromSpv(shaderBase + "test.vert.spv", vk::ShaderStageFlagBits::eVertex)
//		  .AddShaderStageFromSpv(shaderBase + "test.frag.spv", vk::ShaderStageFlagBits::eFragment)
//		  .SetViewportStateDynamic(true)
//		  .SetDepthStencil(false, false, vk::CompareOp::eLess)
//		  .SetMultisample(vk::SampleCountFlagBits::e1)
//		  .SetLayoutCreateInfo(std::vector<vk::DescriptorSetLayout>{}, std::vector<vk::PushConstantRange>{});
//
//		// Use render pass handle from wrapper
//		m_GraphicsPipeline = pb.BuildGraphics(m_RenderPass, 0);
//	}
//	catch (const std::exception& e) {
//		LOG_ERROR("Sandbox: Failed to create renderpass/pipeline: {}", e.what());
//	}
}


void App::OnShutdown()
{
	LOG_INFO("Sandbox: Shutdown.");

	// Destroy pipeline + render pass
//	if (m_GraphicsPipeline)
//		m_GraphicsPipeline.reset();
//	if (m_RenderPass)
//		m_RenderPass.reset();
}
