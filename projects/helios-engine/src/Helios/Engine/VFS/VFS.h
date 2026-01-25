//==============================================================================
// Virtual File System (VFS) public interface
//
// Provides a unified file access layer that abstracts physical file locations
// through mount points and virtual paths. Supports multiple backend
// implementations (physical filesystem, archives, etc.) with priority-based
// mounting, path aliasing (e.g., @textures), and thread-safe operations.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Version history:
// - 2026.01: Initial version
//==============================================================================
#pragma once

#include "Helios/Engine/Util/ScopeRef.h"

#include <map>
#include <mutex>
#include <string>
#include <vector>

// Singleton access macro
#define VirtFS Helios::Engine::VFS::VirtualFileSystem::Get()

namespace Helios::Engine::VFS {


	//------------------------------------------------------------------------------
	// File Handle & Stream
	//------------------------------------------------------------------------------


	enum class FileMode
	{
		Read   = (1 << 0),
		Write  = (1 << 1),
		Append = (1 << 2)
	};


	struct FileHandle
	{
		std::string VirtualPath;
		std::string PhysicalPath;
		size_t Size = 0;
		bool Valid = false;
	};


	class FileStream
	{
	public:
		virtual ~FileStream() = default;

		virtual size_t Read(void* buffer, size_t size) = 0;
		virtual size_t Write(const void* buffer, size_t size) = 0;
		virtual void Close() = 0;

		virtual bool Seek(size_t position) = 0;
		virtual size_t Tell() const = 0;
		virtual size_t Size() const = 0;

		virtual bool IsValid() const = 0;
	};


	//------------------------------------------------------------------------------
	// Backend Interface
	//------------------------------------------------------------------------------


	class VFSBackend
	{
	public:
		virtual ~VFSBackend() = default;

		virtual bool Exists(const std::string& path) const = 0;
		virtual FileHandle GetFileHandle(const std::string& path) const = 0;
		virtual Scope<FileStream> OpenStream(const std::string& path, FileMode mode) = 0;
		virtual std::vector<std::string> ListFiles(const std::string& directory, bool recursive = false) const = 0;
		virtual bool IsDirectory(const std::string& path) const = 0;
	};


	//------------------------------------------------------------------------------
	// Mount Point
	//------------------------------------------------------------------------------


	struct MountPoint
	{
		std::string VirtualPath;    // e.g., "assets"
		std::string ID;             // Unique identifier (for that path) for the mount point (optional, defaults to "default")
		Scope<VFSBackend> Backend;
		int Priority = 0;           // Higher priority checked first
		bool ReadOnly = true;       // Reject write operations if true

		MountPoint(const std::string& virtualPath, Scope<VFSBackend> backend, int priority = 0, const std::string& id = "default")
			: VirtualPath(virtualPath), ID(id), Backend(std::move(backend)), Priority(priority) {}
	};


	//------------------------------------------------------------------------------
	// Virtual File System
	//------------------------------------------------------------------------------


	class VirtualFileSystem
	{
	public:
		VirtualFileSystem() = default;
		~VirtualFileSystem() = default;

		// Singleton access...
		static VirtualFileSystem& Get();

		// Mount management...
		bool Mount(const std::string& virtualPath, const std::string& physicalPath, int priority = 0, const std::string& id = "default");
		bool Mount(const std::string& virtualPath, Scope<VFSBackend> backend, int priority = 0, const std::string& id = "default");
		void Unmount(const std::string& virtualPath, const std::string& id = "default");
		void UnmountAll();
		void UnmountAllWithID(const std::string& id);

		// Alias management...
		bool CreateAlias(const std::string& alias, const std::string& targetPath);
		bool RemoveAlias(const std::string& alias);
		void RemoveAllAliases();
		bool HasAlias(const std::string& alias) const;
		std::string ResolveAlias(const std::string& alias) const;
		std::map<std::string, std::string> GetAllAliases() const;

		// File operations...
		bool Exists(const std::string& virtualPath) const;
		FileHandle GetFileHandle(const std::string& virtualPath) const;
		Scope<FileStream> OpenStream(const std::string& virtualPath, FileMode mode = FileMode::Read);

		// Read...
		std::vector<uint8_t> ReadBinary(const std::string& virtualPath);
		std::string ReadText(const std::string& virtualPath);

		// Write...
		bool WriteBinary(const std::string& virtualPath, const std::vector<uint8_t>& data);
		bool WriteText(const std::string& virtualPath, const std::string& text);

		// Directory...
		std::vector<std::string> ListFiles(const std::string& virtualPath, bool recursive = false) const;
		bool IsDirectory(const std::string& virtualPath) const;

		// Path...
		static std::string NormalizePath(const std::string& path);
		static std::pair<std::string, std::string> SplitPath(const std::string& path);

	private:
		std::vector<MountPoint*> FindMountPoints(const std::string& virtualPath);
		std::vector<const MountPoint*> FindMountPoints(const std::string& virtualPath) const;
		std::string StripMountPrefix(const std::string& virtualPath, const std::string& mountPath) const;
		std::string ResolvePath(const std::string& virtualPath) const;

	private:
		std::vector<MountPoint> m_MountPoints;
		std::map<std::string, std::string> m_Aliases;
		mutable std::mutex m_Mutex;
	};


} // namespace Helios::Engine::VFS
