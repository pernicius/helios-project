function Lib_Platform()
	filter { "kind:*App", "system:windows" }
		links { "user32", "gdi32", "shell32" }
	filter { "kind:*App", "system:linux" }
		links { "pthread", "dl", "X11", "Xxf86vm", "Xrandr", "Xi" }
	filter { "kind:*App", "system:macosx" }
		links { "Cocoa.framework", "IOKit.framework", "CoreVideo.framework" }
	filter {}
end
