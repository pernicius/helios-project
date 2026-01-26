//==============================================================================
// Physical File System Backend
//
// Concrete implementation of VFS backend using the physical file system.
// Provides direct file access through standard filesystem operations with
// root path isolation and path resolution. Supports standard file operations
// including read/write streams, directory listing, and path queries.
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
//
// Main Features:
// - Physical filesystem VFSBackend implementation
// - Root path isolation with std::filesystem integration
// - Thread-safe operations (stateless backend, mutex-protected streams)
// - Full directory operations (create/list/remove/recursive)
// - Binary stream I/O with positioning support
// - Automatic resource management and error handling
// 
// Version history:
// - 2026.01: Added directory creation support
// - 2026.01: Initial version / start of version history
//==============================================================================
#pragma once
#include "Helios/Engine/VFS/VFS.h"

#include <fstream>

namespace Helios::Engine::VFS {


	//------------------------------------------------------------------------------
	// Physical File System Backend
	//------------------------------------------------------------------------------

	class PhysicalFileBackend : public VFSBackend
	{
	public:
		explicit PhysicalFileBackend(const std::string& rootPath);
		virtual ~PhysicalFileBackend() = default;

		// Basic file operations
		bool Exists(const std::string& path) const override;
		FileHandle GetFileHandle(const std::string& path) const override;
		Scope<FileStream> OpenStream(const std::string& path, FileMode mode) override;
		std::vector<std::string> ListFiles(const std::string& directory, bool recursive = false) const override;
		
		// Directory operations
		bool IsDirectory(const std::string& path) const override;
		bool CreateDirectory(const std::string& path) override;
		bool CreateDirectories(const std::string& path) override;
		bool RemoveDirectory(const std::string& path, bool recursive = false) override;

	private:
		std::string ResolvePath(const std::string& path) const;

	private:
		std::string m_RootPath;
	};


	//------------------------------------------------------------------------------
	// Physical File Stream
	//------------------------------------------------------------------------------

	class PhysicalFileStream : public FileStream
	{
	public:
		PhysicalFileStream(const std::string& path, FileMode mode);
		virtual ~PhysicalFileStream();

		size_t Read(void* buffer, size_t size) override;
		size_t Write(const void* buffer, size_t size) override;
		bool Seek(size_t position) override;
		size_t Tell() const override;
		size_t Size() const override;
		bool IsValid() const override;
		void Close() override;

	private:
		mutable std::fstream m_Stream;
		size_t m_Size = 0;
		bool m_Valid = false;
		mutable std::mutex m_Mutex;
	};


} // namespace Helios::Engine::VFS
