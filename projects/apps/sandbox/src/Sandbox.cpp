#include "pch.h"
namespace HE = Helios::Engine;
#include <Helios/Engine/Spec/Spec.h>


class App : public HE::Application
{
public:
	static constexpr int APP_VERSION_MAJOR = 0;
	static constexpr int APP_VERSION_MINOR = 1;
	static constexpr int APP_VERSION_PATCH = 0;
	static constexpr uint32_t APP_VERSION = HE_MAKE_VERSION(APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH);

	App();
	~App();
};


HE::Application* HE::CreateApplication()
{
	{ // Init App Specification
		HE::Spec::App::Name = "Sandbox";
		HE::Spec::App::Version = App::APP_VERSION;
		HE::Spec::App::Hints |= HE::Spec::App::HintFlags::USE_EXEPATH;
		HE::Spec::App::LogFile = "Sandbox.log";
		HE::Spec::App::ConfigFile = "Sandbox.cfg";
	};

	return new App;
}


App::App()
{
//	auto& rendererSpec = HE::DeviceManager::GetSpecification();
//	rendererSpec.Window.windowTitle = appSpec.Name;
//	HE::DeviceManager::SetSpecification(rendererSpec);

	InitRenderer();

//	PushLayer(new SandboxLayer3D());
//	PushOverlay(new ClientUILayer());
// 
//	m_modelTest = Helios::Model::Create();
}


App::~App()
{
	LOG_INFO("Sandbox App Shutdown.");
}
