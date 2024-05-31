project "Indy"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	allmodulespublic "On"
	scanformoduledependencies "true"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"src/**.ixx",
		"lib/ImGui/*.cpp",
		"lib/ImGui/*.h",
		"lib/ImGui/backends/imgui_impl_glfw.cpp",
		"lib/ImGui/backends/imgui_impl_glfw.h",
		"lib/ImGui/backends/imgui_impl_vulkan.cpp",
		"lib/ImGui/backends/imgui_impl_vulkan.h",
	}

	includedirs
	{
		"src",
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Vulkan}",
		"%{IncludeDirs.VMA}",
		"%{IncludeDirs.ImGui}",
	}

	libdirs
	{
		"%{VulkanSDK}/Lib"
	}

	links
	{
		"GLFW",
		"vulkan-1.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ENGINE_PLATFORM_WINDOWS",
			"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS"
		}

	filter "configurations:Debug"
		defines { "ENGINE_DEBUG", "INDY_ENABLE_CORE_DEBUG" }
		runtime "Debug"
		symbols "on"

		links
		{
			"%{VulkanSDK}/Lib/shaderc_shared.lib",
			"%{VulkanSDK}/Lib/spirv-cross-core.lib",
			"%{VulkanSDK}/Lib/spirv-cross-glsl.lib",
		}

	filter "configurations:Release"
		defines { "ENGINE_RELEASE", "INDY_ENABLE_CORE_DEBUG" }
		runtime "Release"
		optimize "on"

		links
		{
			"%{VulkanSDK}/Lib/shaderc_shared.lib",
			"%{VulkanSDK}/Lib/spirv-cross-core.lib",
			"%{VulkanSDK}/Lib/spirv-cross-glsl.lib",
		}

	filter "configurations:Dist"
		defines { "ENGINE_DIST" }
		runtime "Release"
		optimize "on"

		links
		{
			"%{VulkanSDK}/Lib/shaderc_shared.lib",
			"%{VulkanSDK}/Lib/spirv-cross-core.lib",
			"%{VulkanSDK}/Lib/spirv-cross-glsl.lib",
		}