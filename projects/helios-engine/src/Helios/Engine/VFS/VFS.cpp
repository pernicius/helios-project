//==============================================================================
// Virtual File System (VFS) public interface (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Version history:
// - 2026.01: Initial version
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

	VirtualFileSystem& VirtualFileSystem::Get()
	{
		static VirtualFileSystem* s_Instance = nullptr;

		if (!s_Instance) {
			s_Instance = new VirtualFileSystem();
			LOG_CORE_DEBUG("VFS: Virtual File System initialized.");
		}

		return *s_Instance;
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
			LOG_CORE_DEBUG("VFS: Unmounted '{}' with ID '{}'", virtualPath, id);
		}
	}


	void VirtualFileSystem::UnmountAll()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		m_MountPoints.clear();
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
		LOG_CORE_DEBUG("VFS: Created alias '{}' -> '{}'", normalizedAlias, normalizedTarget);
		return true;
	}


	bool VirtualFileSystem::RemoveAlias(const std::string& alias)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		auto it = m_Aliases.find(alias);
		if (it != m_Aliases.end()) {
			m_Aliases.erase(it);
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
		auto mountPoints = FindMountPoints(resolvedPath);

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
		auto mountPoints = FindMountPoints(resolvedPath);

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
		auto mountPoints = FindMountPoints(resolvedPath);

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
		auto mountPoints = FindMountPoints(resolvedPath);

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
		auto mountPoints = FindMountPoints(resolvedPath);

		// Check all matching mount points
		for (const auto* mp : mountPoints) {
			std::string relativePath = StripMountPrefix(resolvedPath, mp->VirtualPath);
			if (mp->Backend->IsDirectory(relativePath)) {
				return true;
			}
		}

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


	//------------------------------------------------------------------------------
	// Private helper methods
	//------------------------------------------------------------------------------

	std::vector<MountPoint*> VirtualFileSystem::FindMountPoints(const std::string& virtualPath)
	{
		std::string normalizedPath = NormalizePath(virtualPath);
		std::vector<MountPoint*> matches;

		// Collect all mount points that match the virtual path
		// Mount points are already sorted by priority and specificity
		for (auto& mp : m_MountPoints) {
			if (normalizedPath.find(mp.VirtualPath) == 0) {
				// Ensure we match at a path boundary
				if (normalizedPath.length() == mp.VirtualPath.length() ||
					normalizedPath[mp.VirtualPath.length()] == '/') {
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
			if (normalizedPath.find(mp.VirtualPath) == 0) {
				if (normalizedPath.length() == mp.VirtualPath.length() ||
					normalizedPath[mp.VirtualPath.length()] == '/') {
					matches.push_back(&mp);
				}
			}
		}

		return matches;
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
					path = it->second;
					if (!remainder.empty() && remainder[0] != '/') {
						path += "/";
					}
					path += remainder;
				}
			}
		}

		return NormalizePath(path);
	}


} // namespace Helios::Engine::VFS
