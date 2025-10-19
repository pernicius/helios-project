#include "pch.h"
#include <Helios/Engine/Renderer/RendererAPI.h>
namespace HE = Helios::Engine;


class App : public HE::Application
{
public:
	static constexpr int APP_VERSION_MAJOR = 0;
	static constexpr int APP_VERSION_MINOR = 1;
	static constexpr int APP_VERSION_PATCH = 0;
	static constexpr uint32_t APP_VERSION = HE_MAKE_VERSION(APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH);

	App(const HE::Application::Specification& spec);
	~App();
};


HE::Application* HE::CreateApplication(HE::Application::CommandLineArgs args)
{
	HE::Application::Specification appSpec{};
	{
		appSpec.Name = "Sandbox";
		appSpec.Version = App::APP_VERSION;
		appSpec.CmdLineArgs = args;
		appSpec.hints |= HE::Application::Hints::HINT_USE_EXEPATH;
		appSpec.logfile = "Sandbox.log";
		appSpec.configfile = "Sandbox.cfg";
	};

	return new App(appSpec);
}


App::App(const HE::Application::Specification& appSpec)
	: HE::Application(appSpec)
{
//	auto& rendererSpec = HE::DeviceManager::GetSpecification();
//	rendererSpec.Window.windowTitle = appSpec.Name;
//	HE::DeviceManager::SetSpecification(rendererSpec);

	auto api = HE::RendererAPI::GetAPI();

	CreateAppWindow();

//	PushLayer(new SandboxLayer3D());
//	PushOverlay(new ClientUILayer());
// 
//	m_modelTest = Helios::Model::Create();
}


App::~App()
{
	LOG_INFO("Sandbox App Shutdown.");
}
