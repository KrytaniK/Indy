project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	systemversion "latest"
	staticruntime "On"

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
	}

	links
	{
		"Indy"
	}

	filter "system:windows"

		defines
		{
			"ENGINE_PLATFORM_WINDOWS"
		}

		includedirs "%{IncludeDirs.GLFW}"

	filter "configurations:Debug"
		defines { "ENGINE_DEBUG" }
		buildoptions "/MDd"
        runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "ENGINE_RELEASE" }
		buildoptions "/MD"
        runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines { "ENGINE_DIST" }
		buildoptions "/MD"
        runtime "Release"
		optimize "On"