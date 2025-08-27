function LibHeliosEngine()
	-- libraries
--	VendorEnTT{}
--	LibLuaHelper{}
--	VendorGlad{}
	VendorGlfw{}
	VendorGlm{}
--	VendorImgui{}
--	VendorLua{}
	VendorSpdlog{}
--	VendorStb{}

	-- renderer support
	VendorVulkan{}
	VendorNVRHI{}

	-- the engine itself
	includedirs "%{wks.location}/projects/Helios-Engine/Engine/source/"
	includedirs "%{wks.location}/projects/shared/"
	links "Helios-Engine"
end


-------------------------------------------------------------------------------
--  External Dependancies
-------------------------------------------------------------------------------


function VendorGlfw()
	links "vendor.glfw"
	includedirs "%{wks.location}/vendor/libs/glfw/include"
	-- Do not include any OpenGL headers
	defines "GLFW_INCLUDE_NONE"
	filter "platforms:Windows"
		defines "GLFW_EXPOSE_NATIVE_WIN32"
	filter {}
end


function VendorGlm()
	includedirs "%{wks.location}/vendor/headers/glm/include"
end


function VendorNVRHI()
	links "nvrhi.common"
	includedirs "%{wks.location}/vendor/libs/nvrhi/include"

	filter "configurations:Debug"
		links "nvrhi.validation"
	
	filter "platforms:Windows"
		links "nvrhi.d3d11"
		links "nvrhi.d3d12"
		links "nvrhi.vulkan"

	filter "platforms:Linux"
		links "nvrhi.vulkan"

	filter "platforms:MacOS"
		links "nvrhi.vulkan"

	filter {}
end


function VendorSpdlog()
	includedirs "%{wks.location}/vendor/headers/spdlog/include/"
end


function VendorVulkan()
	if os.getenv("VULKAN_SDK") then
		includedirs "%VULKAN_SDK%/Include"
		libdirs "%VULKAN_SDK%/Lib"
		links "vulkan-1"
	end
end
