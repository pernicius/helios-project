function LibHeliosEngine()
	-- libraries
--	VendorEnTT{}
--	LibLuaHelper{}
--	VendorGlad{}
--	VendorGlfw{}
--	VendorGlm{}
--	VendorImgui{}
--	VendorLua{}
	VendorSpdlog{}
--	VendorStb{}

	-- renderer support
	VendorDirectX{}
	VendorMetal{}
	VendorVulkan{}
	VendorOpenGL{}

	-- the engine itself
	includedirs "%{wks.location}/projects/Helios-Engine/Engine/source/"
	includedirs "%{wks.location}/projects/shared/"
	links "Helios-Engine"
end

-------------------------------------------------------------------------------
--  External Dependancies
-------------------------------------------------------------------------------


function VendorVulkan()
	if os.getenv("VULKAN_SDK") then
		includedirs "%VULKAN_SDK%/Include"
		libdirs "%VULKAN_SDK%/Lib"
		links "vulkan-1"
	end
end


function VendorOpenGL()
	-- TODO...
	-- TODO...
	-- TODO...
end


function VendorDirectX()
	-- TODO...
	-- TODO...
	-- TODO...
end


function VendorMetal()
	-- TODO...
	-- TODO...
	-- TODO...
end


function VendorSpdlog()
	includedirs "%{wks.location}/vendor/headers/spdlog/include/"
end


function VendorGlfw()
	includedirs "%{wks.location}/vendor/libs/glfw/include"
	defines "GLFW_INCLUDE_NONE"
	links "vendor.glfw"
end
