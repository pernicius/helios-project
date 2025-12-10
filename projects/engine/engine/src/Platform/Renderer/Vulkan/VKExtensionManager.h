#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>

namespace Helios::Engine::Renderer::Vulkan {

	enum class ExtensionScope {
		Instance,
		Layer,
		Device,
	};

	class VKExtensionManager {
	public:
		VKExtensionManager() = default;

		// Register wishes
		void AddRequired(ExtensionScope scope, std::string name);
		void AddOptional(ExtensionScope scope, std::string name);

		// Provide the platform-specific list of available extensions (e.g. from Vulkan enumerate functions)
		void RegisterAvailable(ExtensionScope scope, const std::vector<std::string_view>& available);

		// Resolve decides which extensions will be enabled:
		//  - returns true if all required extensions were found
		//  - returns false if any required extensions are missing (missing names available via GetMissingRequired)
		bool Resolve(ExtensionScope scope);

		// Accessors
		std::vector<std::string> GetEnabled(ExtensionScope scope) const;
		std::vector<std::string> GetMissingRequired(ExtensionScope scope) const;

		// Utility
		void ClearAll();

	private:
		struct Bucket {
			std::unordered_set<std::string> required;
			std::unordered_set<std::string> optional;
			std::unordered_set<std::string> available;
			std::unordered_set<std::string> enabled;
			std::vector<std::string> missingRequired;
		};

		Bucket& GetBucket(ExtensionScope scope);
		const Bucket& GetBucket(ExtensionScope scope) const;

		Bucket m_Instance;
		Bucket m_Layer;
		Bucket m_Device;
	};

} // namespace Helios::Engine::Renderer::Vulkan
