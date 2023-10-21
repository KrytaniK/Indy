-- Workspaces
workspace "Indy"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	VulkanSDK = "C:/VulkanSDK"

	-- Include Directories relative to root folder
	IncludeDirs = {}
	IncludeDirs["spdlog"] = "%{wks.location}/Indy/lib/spdlog/include"
	IncludeDirs["GLFW"] = "%{wks.location}/Indy/lib/GLFW/include"
	IncludeDirs["Vulkan"] = "%{VulkanSDK}/1.3.261.1/Include"


	-- Projects
	include "Indy"
	include "Sandbox"
	include "Indy/lib/GLFW"