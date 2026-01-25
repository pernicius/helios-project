//==============================================================================
// Physical File System Backend (implementation)
//
// Copyright (c) 2026 Lennart "Pernicius" Molnar. All rights reserved.
// Part of the Helios Project - https://github.com/pernicius/helios-project
// 
// Version history:
// - 2026.01: Initial version
//==============================================================================
#include "pch.h"
#include "Helios/Engine/VFS/VFS_PhysFS.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace Helios::Engine::VFS {


	//------------------------------------------------------------------------------
	// PhysicalFileBackend Implementation
	//------------------------------------------------------------------------------

	PhysicalFileBackend::PhysicalFileBackend(const std::string& rootPath)
		: m_RootPath(rootPath)
	{
		// Check for '.' root path and convert to current working directory
		if (m_RootPath == ".") {
			m_RootPath = fs::current_path().string();
		}

		// Normalize and ensure root path ends with separator
		m_RootPath = VirtualFileSystem::NormalizePath(m_RootPath);
		if (!m_RootPath.empty() && m_RootPath.back() != '/') {
			m_RootPath += '/';
		}
		
		LOG_CORE_TRACE("VFS: PhysicalFileBackend created with root: '{}'", m_RootPath);
	}


	std::string PhysicalFileBackend::ResolvePath(const std::string& path) const
	{
		std::string normalized = VirtualFileSystem::NormalizePath(path);
		return m_RootPath + normalized;
	}


	bool PhysicalFileBackend::Exists(const std::string& path) const
	{
		std::string fullPath = ResolvePath(path);
		return fs::exists(fullPath);
	}


	FileHandle PhysicalFileBackend::GetFileHandle(const std::string& path) const
	{
		FileHandle handle;
		handle.VirtualPath = path;
		handle.PhysicalPath = ResolvePath(path);

		try {
			if (fs::exists(handle.PhysicalPath) && !fs::is_directory(handle.PhysicalPath)) {
				handle.Size = static_cast<size_t>(fs::file_size(handle.PhysicalPath));
				handle.Valid = true;
			}
		} catch (const fs::filesystem_error& e) {
			LOG_CORE_ERROR("VFS: Failed to get file handle for '{}': {}", path, e.what());
		}

		return handle;
	}


	Scope<FileStream> PhysicalFileBackend::OpenStream(const std::string& path, FileMode mode)
	{
		std::string fullPath = ResolvePath(path);
		return CreateScope<PhysicalFileStream>(fullPath, mode);
	}


	std::vector<std::string> PhysicalFileBackend::ListFiles(const std::string& directory, bool recursive) const
	{
		std::vector<std::string> files;
		std::string fullPath = ResolvePath(directory);

		if (!fs::exists(fullPath) || !fs::is_directory(fullPath)) {
			return files;
		}

		try {
			if (recursive) {
				for (const auto& entry : fs::recursive_directory_iterator(fullPath)) {
					if (entry.is_regular_file()) {
						std::string relativePath = fs::relative(entry.path(), fullPath).string();
						files.push_back(VirtualFileSystem::NormalizePath(relativePath));
					}
				}
			} else {
				for (const auto& entry : fs::directory_iterator(fullPath)) {
					if (entry.is_regular_file()) {
						files.push_back(entry.path().filename().string());
					}
				}
			}
		} catch (const fs::filesystem_error& e) {
			LOG_CORE_ERROR("VFS: Failed to list files in '{}': {}", directory, e.what());
		}

		return files;
	}


	bool PhysicalFileBackend::IsDirectory(const std::string& path) const
	{
		std::string fullPath = ResolvePath(path);
		return fs::exists(fullPath) && fs::is_directory(fullPath);
	}


	//------------------------------------------------------------------------------
	// PhysicalFileStream Implementation
	//------------------------------------------------------------------------------

	PhysicalFileStream::PhysicalFileStream(const std::string& path, FileMode mode)
	{
		std::ios_base::openmode openMode = std::ios::binary;

		if (static_cast<int>(mode) & static_cast<int>(FileMode::Read)) {
			openMode |= std::ios::in;
		}
		if (static_cast<int>(mode) & static_cast<int>(FileMode::Write)) {
			openMode |= std::ios::out | std::ios::trunc;
		}
		if (static_cast<int>(mode) & static_cast<int>(FileMode::Append)) {
			openMode |= std::ios::out | std::ios::app;
		}

		m_Stream.open(path, openMode);
		m_Valid = m_Stream.is_open();

		if (m_Valid && (static_cast<int>(mode) & static_cast<int>(FileMode::Read))) {
			m_Stream.seekg(0, std::ios::end);
			m_Size = static_cast<size_t>(m_Stream.tellg());
			m_Stream.seekg(0, std::ios::beg);
		}

		if (!m_Valid) {
			LOG_CORE_ERROR("VFS: Failed to open file stream: '{}'", path);
		}
	}


	PhysicalFileStream::~PhysicalFileStream()
	{
		Close();
	}


	size_t PhysicalFileStream::Read(void* buffer, size_t size)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		if (!m_Valid) {
			return 0;
		}

		m_Stream.read(static_cast<char*>(buffer), size);
		return static_cast<size_t>(m_Stream.gcount());
	}


	size_t PhysicalFileStream::Write(const void* buffer, size_t size)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		if (!m_Valid) {
			return 0;
		}

		auto posBefore = m_Stream.tellp();
		m_Stream.write(static_cast<const char*>(buffer), size);
		
		if (!m_Stream.good()) {
			return 0;
		}

		return size;
	}


	bool PhysicalFileStream::Seek(size_t position)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		if (!m_Valid) {
			return false;
		}

		m_Stream.seekg(position, std::ios::beg);
		return m_Stream.good();
	}


	size_t PhysicalFileStream::Tell() const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		if (!m_Valid) {
			return 0;
		}

		return static_cast<size_t>(m_Stream.tellg());
	}


	size_t PhysicalFileStream::Size() const
	{
		return m_Size;
	}


	bool PhysicalFileStream::IsValid() const
	{
		return m_Valid;
	}


	void PhysicalFileStream::Close()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		if (m_Stream.is_open()) {
			m_Stream.close();
		}
		m_Valid = false;
	}


} // namespace Helios::Engine::VFS
