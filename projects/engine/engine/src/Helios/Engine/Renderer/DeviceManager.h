#pragma once

namespace Helios::Engine::Renderer {


	class DeviceManager
	{
	public:
		static Scope<DeviceManager> Create();
		virtual ~DeviceManager() = default;

		virtual void Init() = 0;
	};


} //namespace Helios::Engine::Renderer
