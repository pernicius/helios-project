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
// - 2026.01: Added file read/write stream abstraction
// - 2026.01: Switched to 'Meyer's Singleton' pattern
//            Added directory creation support
// - 2026.01: Added lookup cache with LRU eviction
//            Added overlapping alias support (wildcard matching)
//            Fixed overlapping mount point resolution (hierarchical fallback)
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once

#include "Helios/Engine/Util/ScopeRef.h"

#include <list>
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

// Singleton access macro
#define VirtFS Helios::Engine::VFS::VirtualFileSystem::GetInstance()

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

		// Basic file operations
		virtual bool Exists(const std::string& path) const = 0;
		virtual FileHandle GetFileHandle(const std::string& path) const = 0;
		virtual Scope<FileStream> OpenStream(const std::string& path, FileMode mode) = 0;
		virtual std::vector<std::string> ListFiles(const std::string& directory, bool recursive = false) const = 0;

		// Directory operations
		virtual bool IsDirectory(const std::string& path) const = 0;
		virtual bool CreateDirectory(const std::string& path) = 0;
		virtual bool CreateDirectories(const std::string& path) = 0;
		virtual bool RemoveDirectory(const std::string& path, bool recursive = false) = 0;
	};


	//------------------------------------------------------------------------------
	// Mount Point
	//------------------------------------------------------------------------------


	struct MountPoint
	{
		std::string VirtualPath;    // e.g., "assets"
		Scope<VFSBackend> Backend;
		int Priority = 0;           // Higher priority checked first
		std::string ID;             // Unique identifier (for that path) for the mount point (optional, defaults to "default")
		bool ReadOnly = true;       // Reject write operations if true

		MountPoint(const std::string& virtualPath, Scope<VFSBackend> backend, int priority = 0, const std::string& id = "default", bool readOnly = true)
			: VirtualPath(virtualPath), Backend(std::move(backend)), Priority(priority), ID(id), ReadOnly(readOnly) {}
	};


	//------------------------------------------------------------------------------
	// Lookup Cache Entry
	//------------------------------------------------------------------------------


	struct LookupCacheEntry
	{
		std::string VirtualPath;                     // Resolved virtual path (after alias resolution)
		std::vector<const MountPoint*> MountPoints;  // Matching mount points (in priority order)
		bool Valid = true;                           // Is this cache entry still valid?
	};


	//------------------------------------------------------------------------------
	// Virtual File System
	//------------------------------------------------------------------------------


	class VirtualFileSystem
	{
	public:
		VirtualFileSystem();
		~VirtualFileSystem() = default;

		// Singleton access...
		static VirtualFileSystem& GetInstance();

		// Mount management...
		bool Mount(const std::string& virtualPath, const std::string& physicalPath, int priority = 0, const std::string& id = "default", bool readOnly = true);
		bool Mount(const std::string& virtualPath, Scope<VFSBackend> backend, int priority = 0, const std::string& id = "default", bool readOnly = true);
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
		bool CreateDirectory(const std::string& virtualPath);
		bool CreateDirectories(const std::string& virtualPath);
		bool RemoveDirectory(const std::string& virtualPath, bool recursive = false);

		// Path...
		static std::string NormalizePath(const std::string& path);
		static std::pair<std::string, std::string> SplitPath(const std::string& path);
		static std::string GetParentPath(const std::string& path);
		static std::string GetFileName(const std::string& path);

		// Cache management...
		void ClearCache();
		void SetCacheMaxEntries(size_t maxSize);
		size_t GetCacheSize() const;
		size_t GetCacheHits() const;
		size_t GetCacheMisses() const;
		size_t GetCacheEvictions() const;

	private:
		std::vector<MountPoint*> FindMountPoints(const std::string& virtualPath);
		std::vector<const MountPoint*> FindMountPoints(const std::string& virtualPath) const;
		std::vector<const MountPoint*> FindMountPointsCached(const std::string& virtualPath) const;
		std::string StripMountPrefix(const std::string& virtualPath, const std::string& mountPath) const;
		std::string ResolvePath(const std::string& virtualPath) const;

		// Cache helpers...
		void InvalidateCache();
		void UpdateLRU(const std::string& key) const;
		void EvictOldestCacheEntry() const;

	private:
		std::vector<MountPoint> m_MountPoints;
		std::map<std::string, std::string> m_Aliases;
		mutable std::mutex m_Mutex;

		// Lookup cache (LRU)...
		mutable std::unordered_map<std::string, LookupCacheEntry> m_LookupCache;
		mutable std::list<std::string> m_LRUList;  // Most recently used at front
		mutable size_t m_CacheMaxSize = 256;       // Default: cache up to 256 lookups
		mutable size_t m_CacheHits = 0;
		mutable size_t m_CacheMisses = 0;
		mutable size_t m_CacheEvictions = 0;
	};


} // namespace Helios::Engine::VFS
