project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	systemversion "latest"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"%{wks.location}/Indy/src",
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.Vulkan}",
	}

	libdirs
	{
		"%{VulkanSDK}/1.3.261.1/Lib"
	}

	links
	{
		"Indy",
		"vulkan-1.lib",
	}

	filter "system:windows"

		defines
		{
			"ENGINE_PLATFORM_WINDOWS"
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