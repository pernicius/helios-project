#include "pch.h"
#include "App.h"


Helios::Application* Helios::CreateApplication(Helios::Application::CommandLineArgs args)
{
	Helios::Application::Specification spec{};
	{
		spec.Name = "Helios-Game";
		spec.Version = App::APP_VERSION;
		spec.CmdLineArgs = args;
		spec.hints |= Helios::Application::Hints::HINT_USE_EXEPATH;
		spec.logfile = "Helios-Game.log";
		spec.configfile = "Helios-Game.cfg";
	};

	return new App(spec);
}


App::App(const Helios::Application::Specification& spec)
	: Helios::Application(spec)
{
//	PushLayer(new SandboxLayer3D());
	//	PushOverlay(new ClientUILayer());

	//	m_modelTest = Helios::Model::Create();
}


App::~App()
{
}
