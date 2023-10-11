project "Indy"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
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
		"src",
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Vulkan}",
	}

	libdirs
	{
		"%{prj.directory}/lib/VulkanSDK/1.3.261.1/Lib"
	}

	links
	{
		"GLFW",
		"opengl32.lib",
		"vulkan-1.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ENGINE_PLATFORM_WINDOWS",
			"ENGINE_BUILD_DLL"
		}

		-- It should be noted that if Sandbox has not been built and/or the Sandbox folder
		--	has not been created, this command will fail
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

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