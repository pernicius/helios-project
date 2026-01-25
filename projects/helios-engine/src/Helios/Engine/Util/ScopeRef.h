//==============================================================================
// Smart Pointer Aliases
//
// Provides convenient type aliases and factory functions for std::unique_ptr
// (Scope/CreateScope) and std::shared_ptr (Ref/CreateRef). Uses perfect
// forwarding for efficient construction. Simplifies smart pointer usage
// throughout the engine with shorter, more expressive names that clarify
// ownership semantics (Scope for exclusive ownership, Ref for shared).
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Version history:
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

#include <memory>
#include <utility>

namespace Helios { // start of namespace


	// unique pointers (type)
	template<typename T>
	using Scope = std::unique_ptr<T>;
	// unique pointers (create)
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}


	// shared pointers (type)
	template<typename T>
	using Ref = std::shared_ptr<T>;
	// shared pointers (create)
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}


} // namespace Helios
