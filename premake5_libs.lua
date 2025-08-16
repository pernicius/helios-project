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
	includedirs "%{wks.location}/vendor/headers/spdlog/include"
end
