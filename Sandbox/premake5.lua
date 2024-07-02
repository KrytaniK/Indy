project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	systemversion "latest"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	allmodulespublic "On"
	scanformoduledependencies "true"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"src/**.ixx",
	}

	includedirs
	{
		"%{wks.location}/Indy/src",
		"%{wks.location}/Indy/lib/spdlog/include",
	}

	libdirs
	{
		"%{VulkanSDK}/Lib"
	}

	links
	{
		"Indy",
		"vulkan-1.lib",
		"shaderc_shared.lib",
		"spirv-cross-c-shared.lib",
		"spirv-cross-core.lib",
		"spirv-cross-glsl.lib",
	}

	filter "system:windows"

		defines
		{
			"ENGINE_PLATFORM_WINDOWS",
			"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS"
		}

		includedirs "%{IncludeDirs.GLFW}"

	filter "configurations:Debug"
		defines { "ENGINE_DEBUG" }
        runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines { "ENGINE_RELEASE" }
        runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines { "ENGINE_DIST" }
        runtime "Release"
		optimize "on"