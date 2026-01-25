//==============================================================================
// Virtual File System (VFS) public interface (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Further information in the corresponding header file VFS.h
//==============================================================================
#include "pch.h"
#include "Helios/Engine/VFS/VFS.h"

#include "Helios/Engine/VFS/VFS_PhysFS.h"

#include <algorithm>
#include <sstream>

namespace Helios::Engine::VFS {


	//------------------------------------------------------------------------------
	// VirtualFileSystem Implementation
	//------------------------------------------------------------------------------

	VirtualFileSystem::VirtualFileSystem()
	{
		LOG_CORE_DEBUG("VFS: Virtual File System initialized.");
	}


	VirtualFileSystem& VirtualFileSystem::GetInstance()
	{
		static VirtualFileSystem instance;
		return instance;
	}


	//------------------------------------------------------------------------------
	// Mount management
	//------------------------------------------------------------------------------

	bool VirtualFileSystem::Mount(const std::string& virtualPath, const std::string& physicalPath, int priority, const std::string& id)
	{
		auto backend = CreateScope<PhysicalFileBackend>(physicalPath);
		return Mount(virtualPath, std::move(backend), priority, id);
	}


	bool VirtualFileSystem::Mount(const std::string& virtualPath, Scope<VFSBackend> backend, int priority, const std::string& id)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		std::string normalizedPath = NormalizePath(virtualPath);

		// Check if mount point already exists with same ID
		auto it = std::find_if(m_MountPoints.begin(), m_MountPoints.end(),
			[&normalizedPath, &id](const MountPoint& mp) {
				return (mp.VirtualPath == normalizedPath && mp.ID == id);
			});

		if (it != m_MountPoints.end()) {
			LOG_CORE_WARN("VFS: Mount point '{}' with ID '{}' already exists, replacing.", virtualPath, id);
			m_MountPoints.erase(it);
		}

		m_MountPoints.emplace_back(normalizedPath, std::move(backend), priority, id);

		// Sort by priority (higher first), then by path length (longer first for specificity)
		std::sort(m_MountPoints.begin(), m_MountPoints.end(),
			[](const MountPoint& a, const MountPoint& b) {
				if (a.Priority != b.Priority) {
					return a.Priority > b.Priority;
				}
				return a.VirtualPath.length() > b.VirtualPath.length();
			});

		// Invalidate cache when mount points change
		InvalidateCache();

		LOG_CORE_DEBUG("VFS: Mounted '{}' with ID '{}' and priority {}", virtualPath, id, priority);
		return true;
	}


	void VirtualFileSystem::Unmount(const std::string& virtualPath, const std::string& id)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		std::string normalizedPath = NormalizePath(virtualPath);

		auto it = std::remove_if(m_MountPoints.begin(), m_MountPoints.end(),
			[&normalizedPath, &id](const MountPoint& mp) {
				return (mp.VirtualPath == normalizedPath && mp.ID == id);
			});

		if (it != m_MountPoints.end()) {
			m_MountPoints.erase(it, m_MountPoints.end());
			
			// Invalidate cache when mount points change
			InvalidateCache();
			
			LOG_CORE_DEBUG("VFS: Unmounted '{}' with ID '{}'", virtualPath, id);
		}
	}


	void VirtualFileSystem::UnmountAll()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		m_MountPoints.clear();
		
		// Invalidate cache when mount points change
		InvalidateCache();
		
		LOG_CORE_DEBUG("VFS: All mount points cleared");
	}


	void VirtualFileSystem::UnmountAllWithID(const std::string& id)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		auto it = std::remove_if(m_MountPoints.begin(), m_MountPoints.end(),
			[&id](const MountPoint& mp) {
				return mp.ID == id;
			});

		if (it != m_MountPoints.end()) {
			m_MountPoints.erase(it, m_MountPoints.end());
			
			// Invalidate cache when mount points change
			InvalidateCache();
			
			LOG_CORE_DEBUG("VFS: Unmounted all with ID '{}'", id);
		}
	}


	//------------------------------------------------------------------------------
	// Alias management
	//------------------------------------------------------------------------------

	bool VirtualFileSystem::CreateAlias(const std::string& alias, const std::string& targetPath)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		// Check for '@' prefix
		std::string normalizedAlias = alias;
		if (normalizedAlias.empty() || normalizedAlias[0] != '@') {
			LOG_CORE_WARN("VFS: Alias '{}' should start with '@', adding automatically.", alias);
			normalizedAlias = "@" + normalizedAlias;
		}

		// Check for ':' postfix
		if (normalizedAlias.empty() || normalizedAlias.back() != ':') {
			LOG_CORE_WARN("VFS: Alias '{}' should end with ':', adding automatically.", alias);
			normalizedAlias += ":";
		}

		std::string normalizedTarget = NormalizePath(targetPath);

		if (m_Aliases.find(normalizedAlias) != m_Aliases.end()) {
			LOG_CORE_WARN("VFS: Alias '{}' already exists, replacing.", normalizedAlias);
		}

		m_Aliases[normalizedAlias] = normalizedTarget;
		
		// Invalidate cache when aliases change
		InvalidateCache();
		
		LOG_CORE_DEBUG("VFS: Created alias '{}' -> '{}'", normalizedAlias, normalizedTarget);
		return true;
	}


	bool VirtualFileSystem::RemoveAlias(const std::string& alias)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		auto it = m_Aliases.find(alias);
		if (it != m_Aliases.end()) {
			m_Aliases.erase(it);
			
			// Invalidate cache when aliases change
			InvalidateCache();
			
			LOG_CORE_DEBUG("VFS: Removed alias '{}'", alias);
			return true;
		}

		LOG_CORE_WARN("VFS: Alias '{}' not found", alias);
		return false;
	}


	void VirtualFileSystem::RemoveAllAliases()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		m_Aliases.clear();
		
		// Invalidate cache when aliases change
		InvalidateCache();
		
		LOG_CORE_DEBUG("VFS: All aliases removed");
	}


	bool VirtualFileSystem::HasAlias(const std::string& alias) const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		return m_Aliases.find(alias) != m_Aliases.end();
	}


	std::string VirtualFileSystem::ResolveAlias(const std::string& alias) const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		auto it = m_Aliases.find(alias);
		if (it != m_Aliases.end()) {
			return it->second;
		}

		return alias;
	}


	std::map<std::string, std::string> VirtualFileSystem::GetAllAliases() const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		return m_Aliases;
	}


	//------------------------------------------------------------------------------
	// File operations
	//------------------------------------------------------------------------------

	bool VirtualFileSystem::Exists(const std::string& virtualPath) const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		std::string resolvedPath = ResolvePath(virtualPath);
		auto mountPoints = FindMountPointsCached(resolvedPath);

		// Check all matching mount points (higher priority first due to sorting)
		for (const auto* mp : mountPoints) {
			std::string relativePath = StripMountPrefix(resolvedPath, mp->VirtualPath);
			if (mp->Backend->Exists(relativePath)) {
				return true;
			}
		}

		return false;
	}


	FileHandle VirtualFileSystem::GetFileHandle(const std::string& virtualPath) const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		std::string resolvedPath = ResolvePath(virtualPath);
		auto mountPoints = FindMountPointsCached(resolvedPath);

		// Check all matching mount points (higher priority first)
		for (const auto* mp : mountPoints) {
			std::string relativePath = StripMountPrefix(resolvedPath, mp->VirtualPath);
			FileHandle handle = mp->Backend->GetFileHandle(relativePath);
			if (handle.Valid) {
				return handle;
			}
		}

		LOG_CORE_WARN("VFS: File not found in any mount point: '{}'", virtualPath);
		return FileHandle();
	}


	Scope<FileStream> VirtualFileSystem::OpenStream(const std::string& virtualPath, FileMode mode)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		std::string resolvedPath = ResolvePath(virtualPath);
		auto mountPoints = FindMountPointsCached(resolvedPath);

		if (mountPoints.empty()) {
			LOG_CORE_ERROR("VFS: No mount point found for '{}'", virtualPath);
			return nullptr;
		}

		// For write operations, use the highest-priority writable mount point
		if (mode != FileMode::Read) {
			for (auto* mp : mountPoints) {
				if (mp->ReadOnly) {
					continue; // Skip read-only mounts
				}

				std::string relativePath = StripMountPrefix(resolvedPath, mp->VirtualPath);
				return mp->Backend->OpenStream(relativePath, mode);
			}

			LOG_CORE_ERROR("VFS: No writable mount point found for '{}'", virtualPath);
			return nullptr;
		}

		// For read operations, try all mount points until one succeeds
		for (auto* mp : mountPoints) {
			std::string relativePath = StripMountPrefix(resolvedPath, mp->VirtualPath);

			// Check if file exists first to avoid unnecessary stream creation
			if (!mp->Backend->Exists(relativePath)) {
				continue;
			}

			auto stream = mp->Backend->OpenStream(relativePath, mode);
			if (stream && stream->IsValid()) {
				return stream;
			}
		}

		LOG_CORE_ERROR("VFS: Failed to open stream for '{}'", virtualPath);
		return nullptr;
	}


	//------------------------------------------------------------------------------
	// Read operations
	//------------------------------------------------------------------------------

	std::vector<uint8_t> VirtualFileSystem::ReadBinary(const std::string& virtualPath)
	{
		auto stream = OpenStream(virtualPath, FileMode::Read);
		if (!stream || !stream->IsValid()) {
			LOG_CORE_ERROR("VFS: Failed to open file '{}'", virtualPath);
			return {};
		}

		size_t fileSize = stream->Size();
		std::vector<uint8_t> buffer(fileSize);

		size_t bytesRead = stream->Read(buffer.data(), fileSize);
		if (bytesRead != fileSize) {
			LOG_CORE_WARN("VFS: Read {} bytes from '{}', expected {}", bytesRead, virtualPath, fileSize);
			buffer.resize(bytesRead);
		}

		stream->Close();
		return buffer;
	}


	std::string VirtualFileSystem::ReadText(const std::string& virtualPath)
	{
		auto data = ReadBinary(virtualPath);
		return std::string(data.begin(), data.end());
	}


	//------------------------------------------------------------------------------
	// Write operations
	//------------------------------------------------------------------------------

	bool VirtualFileSystem::WriteBinary(const std::string& virtualPath, const std::vector<uint8_t>& data)
	{
		auto stream = OpenStream(virtualPath, FileMode::Write);
		if (!stream || !stream->IsValid()) {
			LOG_CORE_ERROR("VFS: Failed to open file '{}' for writing", virtualPath);
			return false;
		}

		size_t bytesWritten = stream->Write(data.data(), data.size());
		stream->Close();

		return bytesWritten == data.size();
	}


	bool VirtualFileSystem::WriteText(const std::string& virtualPath, const std::string& text)
	{
		std::vector<uint8_t> data(text.begin(), text.end());
		return WriteBinary(virtualPath, data);
	}


	//------------------------------------------------------------------------------
	// Directory operations
	//------------------------------------------------------------------------------

	std::vector<std::string> VirtualFileSystem::ListFiles(const std::string& virtualPath, bool recursive) const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		std::string resolvedPath = ResolvePath(virtualPath);
		auto mountPoints = FindMountPointsCached(resolvedPath);

		std::vector<std::string> allFiles;

		// Collect files from ALL matching mount points (supports overlapping)
		for (const auto* mp : mountPoints) {
			std::string relativePath = StripMountPrefix(resolvedPath, mp->VirtualPath);
			auto files = mp->Backend->ListFiles(relativePath, recursive);

			// Merge unique files
			for (const auto& file : files) {
				if (std::find(allFiles.begin(), allFiles.end(), file) == allFiles.end()) {
					allFiles.push_back(file);
				}
			}
		}

		return allFiles;
	}


	bool VirtualFileSystem::IsDirectory(const std::string& virtualPath) const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		std::string resolvedPath = ResolvePath(virtualPath);
		auto mountPoints = FindMountPointsCached(resolvedPath);

		// Check all matching mount points
		for (const auto* mp : mountPoints) {
			std::string relativePath = StripMountPrefix(resolvedPath, mp->VirtualPath);
			if (mp->Backend->IsDirectory(relativePath)) {
				return true;
			}
		}

		return false;
	}


	bool VirtualFileSystem::CreateDirectory(const std::string& virtualPath)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		std::string resolvedPath = ResolvePath(virtualPath);
		auto mountPoints = FindMountPointsCached(resolvedPath);

		if (mountPoints.empty()) {
			LOG_CORE_ERROR("VFS: No mount point found for '{}'", virtualPath);
			return false;
		}

		// Use the highest-priority writable mount point
		for (auto* mp : mountPoints) {
			if (mp->ReadOnly) {
				continue;
			}

			std::string relativePath = StripMountPrefix(resolvedPath, mp->VirtualPath);
			bool result = mp->Backend->CreateDirectory(relativePath);

			if (result) {
				LOG_CORE_DEBUG("VFS: Created directory '{}'", virtualPath);
			}

			return result;
		}

		LOG_CORE_ERROR("VFS: No writable mount point found for '{}'", virtualPath);
		return false;
	}


	bool VirtualFileSystem::CreateDirectories(const std::string& virtualPath)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		std::string resolvedPath = ResolvePath(virtualPath);
		auto mountPoints = FindMountPointsCached(resolvedPath);

		if (mountPoints.empty()) {
			LOG_CORE_ERROR("VFS: No mount point found for '{}'", virtualPath);
			return false;
		}

		// Use the highest-priority writable mount point
		for (auto* mp : mountPoints) {
			if (mp->ReadOnly) {
				continue;
			}

			std::string relativePath = StripMountPrefix(resolvedPath, mp->VirtualPath);
			bool result = mp->Backend->CreateDirectories(relativePath);

			if (result) {
				LOG_CORE_DEBUG("VFS: Created directories '{}'", virtualPath);
			}

			return result;
		}

		LOG_CORE_ERROR("VFS: No writable mount point found for '{}'", virtualPath);
		return false;
	}


	bool VirtualFileSystem::RemoveDirectory(const std::string& virtualPath, bool recursive)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		std::string resolvedPath = ResolvePath(virtualPath);
		auto mountPoints = FindMountPointsCached(resolvedPath);

		if (mountPoints.empty()) {
			LOG_CORE_ERROR("VFS: No mount point found for '{}'", virtualPath);
			return false;
		}

		// Use the highest-priority writable mount point
		for (auto* mp : mountPoints) {
			if (mp->ReadOnly) {
				continue;
			}

			std::string relativePath = StripMountPrefix(resolvedPath, mp->VirtualPath);
			bool result = mp->Backend->RemoveDirectory(relativePath, recursive);

			if (result) {
				LOG_CORE_DEBUG("VFS: Removed directory '{}'", virtualPath);
			}

			return result;
		}

		LOG_CORE_ERROR("VFS: No writable mount point found for '{}'", virtualPath);
		return false;
	}


	//------------------------------------------------------------------------------
	// Path utilities
	//------------------------------------------------------------------------------

	std::string VirtualFileSystem::NormalizePath(const std::string& path)
	{
		if (path.empty()) {
			return "";
		}

		std::string normalized = path;

		// Replace backslashes with forward slashes
		std::replace(normalized.begin(), normalized.end(), '\\', '/');

		// Resolve ".." and "." components
		std::vector<std::string> parts;
		std::istringstream stream(normalized);
		std::string part;

		while (std::getline(stream, part, '/')) {
			if (part.empty() || part == ".") {
				continue; // Skip empty and current directory markers
			} else if (part == "..") {
				if (!parts.empty() && parts.back() != "..") {
					parts.pop_back(); // Go up one directory
				} else {
					parts.push_back(part); // Keep .. if at root
				}
			} else {
				parts.push_back(part);
			}
		}

		// Reconstruct path
		std::ostringstream result;
		for (size_t i = 0; i < parts.size(); ++i) {
			if (i > 0) {
				result << "/";
			}
			result << parts[i];
		}

		return result.str();
	}


	std::pair<std::string, std::string> VirtualFileSystem::SplitPath(const std::string& path)
	{
		std::string normalized = NormalizePath(path);
		size_t lastSlash = normalized.find_last_of('/');

		if (lastSlash == std::string::npos) {
			return { "", normalized };
		}

		return {
			normalized.substr(0, lastSlash),
			normalized.substr(lastSlash + 1)
		};
	}


	std::string VirtualFileSystem::GetParentPath(const std::string& path)
	{
		auto [parent, filename] = SplitPath(path);
		return parent;
	}


	std::string VirtualFileSystem::GetFileName(const std::string& path)
	{
		auto [parent, filename] = SplitPath(path);
		return filename;
	}


	//------------------------------------------------------------------------------
	// Cache management
	//------------------------------------------------------------------------------

	void VirtualFileSystem::ClearCache()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		m_LookupCache.clear();
		m_LRUList.clear();
		m_CacheHits = 0;
		m_CacheMisses = 0;

		LOG_CORE_DEBUG("VFS: Lookup cache cleared");
	}


	void VirtualFileSystem::SetCacheMaxEntries(size_t maxSize)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		m_CacheMaxSize = maxSize;

		// Evict excess entries if new size is smaller
		while (m_LookupCache.size() > m_CacheMaxSize) {
			EvictOldestCacheEntry();
		}

		LOG_CORE_DEBUG("VFS: Cache max size set to {}", maxSize);
	}


	size_t VirtualFileSystem::GetCacheSize() const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		return m_LookupCache.size();
	}


	size_t VirtualFileSystem::GetCacheHits() const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		return m_CacheHits;
	}


	size_t VirtualFileSystem::GetCacheMisses() const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		return m_CacheMisses;
	}


	size_t VirtualFileSystem::GetCacheEvictions() const
	{
		return m_CacheEvictions;
	}


	//------------------------------------------------------------------------------
	// Private helper methods
	//------------------------------------------------------------------------------

	std::vector<MountPoint*> VirtualFileSystem::FindMountPoints(const std::string& virtualPath)
	{
		std::string normalizedPath = NormalizePath(virtualPath);
		std::vector<MountPoint*> matches;

		// Collect all mount points that match the virtual path hierarchically
		// Mount points are already sorted by priority and specificity (length)
		for (auto& mp : m_MountPoints) {
			// Check if the mount point is a prefix of the path (or exact match)
			if (normalizedPath == mp.VirtualPath) {
				// Exact match
				matches.push_back(&mp);
			} else if (normalizedPath.find(mp.VirtualPath) == 0) {
				// Prefix match - ensure it's at a path boundary
				size_t prefixLen = mp.VirtualPath.length();
				if (normalizedPath[prefixLen] == '/') {
					matches.push_back(&mp);
				}
			}
		}

		return matches;
	}


	std::vector<const MountPoint*> VirtualFileSystem::FindMountPoints(const std::string& virtualPath) const
	{
		std::string normalizedPath = NormalizePath(virtualPath);
		std::vector<const MountPoint*> matches;

		for (const auto& mp : m_MountPoints) {
			// Check if the mount point is a prefix of the path (or exact match)
			if (normalizedPath == mp.VirtualPath) {
				// Exact match
				matches.push_back(&mp);
			} else if (normalizedPath.find(mp.VirtualPath) == 0) {
				// Prefix match - ensure it's at a path boundary
				size_t prefixLen = mp.VirtualPath.length();
				if (normalizedPath[prefixLen] == '/') {
					matches.push_back(&mp);
				}
			}
		}

		return matches;
	}


	std::vector<const MountPoint*> VirtualFileSystem::FindMountPointsCached(const std::string& virtualPath) const
	{
		// Check if we have a cached result
		auto cacheIt = m_LookupCache.find(virtualPath);
		if (cacheIt != m_LookupCache.end() && cacheIt->second.Valid) {
			// Cache hit - update LRU and return cached result
			++m_CacheHits;
			UpdateLRU(virtualPath);
			return cacheIt->second.MountPoints;
		}

		// Cache miss - perform actual lookup
		++m_CacheMisses;
		auto mountPoints = FindMountPoints(virtualPath);

		// Store result in cache
		if (m_LookupCache.size() >= m_CacheMaxSize) {
			EvictOldestCacheEntry();
		}

		LookupCacheEntry entry;
		entry.VirtualPath = virtualPath;
		entry.MountPoints = mountPoints;
		entry.Valid = true;

		m_LookupCache[virtualPath] = entry;
		m_LRUList.push_front(virtualPath);

		return mountPoints;
	}


	std::string VirtualFileSystem::StripMountPrefix(const std::string& virtualPath, const std::string& mountPath) const
	{
		std::string normalized = NormalizePath(virtualPath);
		
		if (normalized.find(mountPath) == 0) {
			std::string result = normalized.substr(mountPath.length());
			// Remove leading slash if present
			if (!result.empty() && result[0] == '/') {
				result = result.substr(1);
			}
			return result;
		}

		return normalized;
	}


	std::string VirtualFileSystem::ResolvePath(const std::string& virtualPath) const
	{
		std::string path = virtualPath;

		// Check if path starts with an alias
		if (!path.empty() && path[0] == '@') {
			size_t colonPos = path.find(':');
			if (colonPos != std::string::npos) {
				std::string aliasName = path.substr(0, colonPos + 1);
				std::string remainder = path.substr(colonPos + 1);

				auto it = m_Aliases.find(aliasName);
				if (it != m_Aliases.end()) {
					// Alias found - resolve to target path
					path = it->second;
					
					// Handle remainder (the part after the alias)
					if (!remainder.empty()) {
						// If remainder starts with '/', we're explicitly specifying a sub-path
						// Example: @textures:/ui/button.png -> textures/ui/button.png
						if (remainder[0] == '/') {
							remainder = remainder.substr(1); // Strip leading slash
						}
						
						// Append remainder to alias target
						if (!path.empty() && path.back() != '/') {
							path += "/";
						}
						path += remainder;
					}
					// else: No remainder means we're accessing the alias root directly
					//       Example: @textures: -> textures/
				}
				// else: Alias not found - leave path unchanged for error handling
			}
		}

		return NormalizePath(path);
	}


	void VirtualFileSystem::InvalidateCache()
	{
		// Mark all cache entries as invalid (will be cleared on next access)
		m_LookupCache.clear();
		m_LRUList.clear();
		
		LOG_CORE_TRACE("VFS: Lookup cache invalidated");
	}


	void VirtualFileSystem::UpdateLRU(const std::string& key) const
	{
		// Remove key from its current position in LRU list
		auto it = std::find(m_LRUList.begin(), m_LRUList.end(), key);
		if (it != m_LRUList.end()) {
			m_LRUList.erase(it);
		}

		// Add key to front (most recently used)
		m_LRUList.push_front(key);
	}


	void VirtualFileSystem::EvictOldestCacheEntry() const
	{
		if (m_LRUList.empty()) {
			return;
		}

		// Remove least recently used entry (back of list)
		const std::string& oldestKey = m_LRUList.back();
		m_LookupCache.erase(oldestKey);
		m_LRUList.pop_back();
		++m_CacheEvictions;
	}


} // namespace Helios::Engine::VFS
