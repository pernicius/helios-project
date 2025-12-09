#pragma once

namespace Helios::Engine::Renderer {


	class DeviceManager
	{
	public:
		static Ref<DeviceManager> Create();
		virtual ~DeviceManager() = default;
	};


} //namespace Helios::Engine::Renderer
