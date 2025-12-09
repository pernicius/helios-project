#pragma once

namespace Helios::Engine::Renderer::Vulkan {


	using ExtensionSet = std::unordered_set<std::string>;
	struct ExtensionStruct {
		ExtensionSet required;
		ExtensionSet optional;
		ExtensionSet supported;
		ExtensionSet enabled;
	};


} // namespace Helios::Engine::Renderer::Vulkan

