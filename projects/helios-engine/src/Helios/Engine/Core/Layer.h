#pragma once

namespace Helios::Engine {


	class Layer
	{
	public:
		Layer(const std::string& name = "Layer")
			: m_Name(name)
		{
		}
		virtual ~Layer() = default;
		[[nodiscard]] const std::string& GetName() const { return m_Name; }

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		
		virtual void OnUpdate(float /*timestep*/) {}
		virtual void OnRender() {}

		virtual void OnEvent(class Event& /*event*/) {}
		
	protected:
		std::string m_Name;
	};


} // namespace Helios::Engine
