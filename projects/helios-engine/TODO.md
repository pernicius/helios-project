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

central configuration solution
 - ConfigManager class
 - use ConfigManager insead of direct IniParcer calls
 - hirarchy of config files (default (read-only), platform (read-only), user)


better comments:
 - descriptive file headers (see: VFS/VFS.h)
   - short description
   - key features
   - usage
 - in general more comments
 - doxygen style comments for all (public) classes and functions


better implementation of an VFS (virtual file system)
 - path normalization
   - "test\test\" -> "test/test/"
   - "test/../test2/" -> "test2/"
 - multiple backends
   - PhysFS
   - ArchFS (ZIP, 7z, PAK),
   - MemFS
 - overlapping files/paths
   - example: mount point 1 virt. path "textures/"
   - example: mount point 2 virt. path "textures/engine/"
   - example: mount point 3 virt. path "textures/UI/"
   - example: mount point 4 virt. path "textures/"
   - example: virt. file "textures/test.txt" will result in an search in mount points 1 and 4
 - overlapping aliases
   - example: alias "@textures:" -> will search for files in mount points 1, 2, 3, 4
 - file cache (file content)
 - lookup cache (lookup results for faster re-access)
   - maps virt. paths to mount points
