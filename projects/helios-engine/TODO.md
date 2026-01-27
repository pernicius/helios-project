> [!NOTE]
> This is just a reminder for myself where i'm at and what are the next steps i want to do.


Window handling:
 - Handle non-default video-Mode for fullscreen windows
 (see Window.cpp line 182 in DetermineTargetMonitor())
 - window icons
 - window title
 - custom decorations (later)

Next steps in vulkan:
 - [ ] DeviceManager
 - [ ] instance
 - [ ] physical device
 - [ ] logical device
 - [ ] swapchain
 - [ ] pipeline
 - [ ] basic rendering
 - [ ] multithreaded rendering
 - [ ] ....


App icon: (Copilot questions)
 - how can i add an icon to my application
 - How can I ensure that the icon appears correctly on all platforms when using GLFW?
 - Windows: resource files?
 - Linux: ???
 - MaxOS: ???

Rework CONTRIBUTING.md
 - coding style

Rework README.md

better comments:
 - descriptive file headers
   - usage
 - in general more comments
 - doxygen style comments for all (public) classes and functions


better implementation of an VFS (virtual file system)
 - multiple backends
   - ArchFS (ZIP, 7z, PAK),
   - MemFS
 - file cache (file content)
