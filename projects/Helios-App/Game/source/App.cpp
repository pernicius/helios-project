#include "pch.h"
#include "App.h"


Helios::Engine::Application* Helios::Engine::CreateApplication(Helios::Engine::Application::CommandLineArgs args)
{
	Helios::Engine::Application::Specification spec{};
	{
		spec.Name = "Helios-Game";
		spec.Version = App::APP_VERSION;
		spec.CmdLineArgs = args;
		spec.hints |= Helios::Engine::Application::Hints::HINT_USE_EXEPATH;
		spec.logfile = "Helios-Game.log";
		spec.configfile = "Helios-Game.cfg";
	};

	return new App(spec);
}


App::App(const Helios::Engine::Application::Specification& spec)
	: Helios::Engine::Application(spec)
{
//	PushLayer(new SandboxLayer3D());
	//	PushOverlay(new ClientUILayer());

	//	m_modelTest = Helios::Model::Create();
}


App::~App()
{
}
