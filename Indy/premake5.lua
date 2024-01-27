project "Indy"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"src/**.ixx",
	}

	includedirs
	{
		"src",
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Vulkan}",
	}

	libdirs
	{
		"%{VulkanSDK}/1.3.261.1/Lib"
	}

	links
	{
		"GLFW",
		"vulkan-1.lib",
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

	filter "configurations:Release"
		defines { "ENGINE_RELEASE", "INDY_ENABLE_CORE_DEBUG" }
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines { "ENGINE_DIST" }
		runtime "Release"
		optimize "on"