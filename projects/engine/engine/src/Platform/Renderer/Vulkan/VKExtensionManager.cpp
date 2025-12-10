#include "pch.h"
#include "Platform/Renderer/Vulkan/VKExtensionManager.h"

#include <algorithm>
#include <stdexcept>

namespace Helios::Engine::Renderer::Vulkan {


	static inline std::string ToString(std::string_view sv)
	{
		return std::string(sv);
	}


	VKExtensionManager::Bucket& VKExtensionManager::GetBucket(ExtensionScope scope)
	{
		switch (scope) {
		case ExtensionScope::Instance: return m_Instance;
		case ExtensionScope::Layer: return m_Layer;
		case ExtensionScope::Device: return m_Device;
		}
		throw std::runtime_error("ExtensionManager: invalid scope");
	}


	const VKExtensionManager::Bucket& VKExtensionManager::GetBucket(ExtensionScope scope) const
	{
		switch (scope) {
		case ExtensionScope::Instance: return m_Instance;
		case ExtensionScope::Layer: return m_Layer;
		case ExtensionScope::Device: return m_Device;
		}
		throw std::runtime_error("ExtensionManager: invalid scope");
	}


	void VKExtensionManager::AddRequired(ExtensionScope scope, std::string name)
	{
		auto& b = GetBucket(scope);
		b.required.insert(std::move(name));
	}


	void VKExtensionManager::AddOptional(ExtensionScope scope, std::string name)
	{
		auto& b = GetBucket(scope);
		b.optional.insert(std::move(name));
	}


	void VKExtensionManager::RegisterAvailable(ExtensionScope scope, const std::vector<std::string_view>& available)
	{
		auto& b = GetBucket(scope);
		b.available.clear();
		for (auto sv : available) {
			b.available.insert(ToString(sv));
		}
	}


	bool VKExtensionManager::Resolve(ExtensionScope scope)
	{
		auto& b = GetBucket(scope);
		b.enabled.clear();
		b.missingRequired.clear();

		// Check required extensions
		for (const auto& req : b.required) {
			if (b.available.find(req) == b.available.end()) {
				b.missingRequired.push_back(req);
			}
			else {
				b.enabled.insert(req);
			}
		}

		// If any required missing -> fail
		if (!b.missingRequired.empty()) {
			// keep enabled list only for the ones that exist; caller can inspect missing list
			return false;
		}

		// Try to enable optional extensions if available
		for (const auto& opt : b.optional) {
			if (b.available.find(opt) != b.available.end()) {
				b.enabled.insert(opt);
			}
		}

		return true;
	}


	std::vector<std::string> VKExtensionManager::GetEnabled(ExtensionScope scope) const
	{
		const auto& b = GetBucket(scope);
		std::vector<std::string> out;
		out.reserve(b.enabled.size());
		for (const auto& e : b.enabled) out.push_back(e);
		std::sort(out.begin(), out.end());
		return out;
	}


	std::vector<std::string> VKExtensionManager::GetMissingRequired(ExtensionScope scope) const
	{
		const auto& b = GetBucket(scope);
		auto out = b.missingRequired;
		std::sort(out.begin(), out.end());
		return out;
	}


	void VKExtensionManager::ClearAll()
	{
		m_Instance = Bucket{};
		m_Layer = Bucket{};
		m_Device = Bucket{};
	}


} // namespace Helios::Engine::Renderer::Vulkan
