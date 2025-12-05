#pragma once

namespace Helios::Engine::Renderer {


	class Context
	{
	public:
		static Scope<Context> Create();
		virtual ~Context() = default;

		virtual void Init() = 0;
	};


} //namespace Helios::Engine::Renderer
