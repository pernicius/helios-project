#include "pch.h"
#include "App.h"

namespace HE = Helios::Engine;

HE::Application* Helios::Engine::CreateApplication(HE::Application::CommandLineArgs args)
{
	HE::Application::Specification appSpec{};
	{
		appSpec.Name = "HeliosGame";
		appSpec.Version = App::APP_VERSION;
		appSpec.CmdLineArgs = args;
		appSpec.hints |= HE::Application::Hints::HINT_USE_EXEPATH;
		appSpec.logfile = "Game.log";
		appSpec.configfile = "Game.cfg";
	};

	return new App(appSpec);
}


App::App(const HE::Application::Specification& appSpec)
	: HE::Application(appSpec)
{
	auto& rendererSpec = HE::DeviceManager::GetSpecification();
	rendererSpec.Window.windowTitle = appSpec.Name;
	HE::DeviceManager::SetSpecification(rendererSpec);

	CreateAppWindow();

//	PushLayer(new SandboxLayer3D());
//	PushOverlay(new ClientUILayer());
//	m_modelTest = Helios::Model::Create();
}


App::~App()
{
}
