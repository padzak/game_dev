local ROOT = "../"    -- Path to project root

workspace "ArcadeApp"
	architecture "x64"
	startproject "ArcadeApp"

	configurations
	{
		"Debug",
		"Release",
	}

outputdir = "%{cfg.buildcfg}_%{cfg.system}_%{cfg.architecture}"

includeDirs = {}


project "ArcadeApp"
	location "ArcadeApp"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	local output_dir_root	= "bin/" .. outputdir .. "/%{prj.name}"
	local obj_dir_root = "bin_int/" .. outputdir .. "/%{prj.name}"

	targetdir (output_dir_root)
	objdir (obj_dir_root)
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/src/**",
	}

	filter "system:windows"
		systemversion "latest"
		disablewarnings { "26812", "26495" }

		libdirs
		{
			"%{prj.name}/lib/SDL2/Windows/lib/x64",
		}

		includedirs
		{
			"%{prj.name}/lib/SDL2/Windows/include",
		}

		links
		{
			"SDL2",
		}

	filter {"system:windows", "action:gmake2"}
		postbuildcommands
		{
			"cp ./lib/SDL2/Windows/lib/x64/SDL2.dll" .. " ../" .. output_dir_root,
			"cp -rf ./assets" .. " ../" .. output_dir_root .. "/assets/"
		}

	filter {"system:windows"}
		postbuildcommands
		{
			"{COPY} ./lib/SDL2/Windows/lib/x64/SDL2.dll" .. " ../" .. output_dir_root,
			"{RMDIR} ../" .. output_dir_root .. "/assets/",
			"{MKDIR} ../" .. output_dir_root .. "/assets/",
			"{COPY} ./assets" .. " ../" .. output_dir_root .. "/assets/"
		}


	filter "system:macosx"
		systemversion "10.13"


		buildoptions
		{
			"-F ./lib/SDL2/MacOSX/"
		}

		linkoptions
		{
			"-F ./lib/SDL2/MacOSX/",
		}

		links
		{
			"SDL2.framework"
		}

		postbuildcommands
		{
			"{RMDIR} ../" .. output_dir_root .. "/assets/",
			"{COPY} ./assets" .. " ../" .. output_dir_root
		}
		
	filter {"action:gmake2", "system:linux"}
		links
		{
			"SDL2"	
		}
		
		postbuildcommands
		{
			"{RMDIR} ../" .. output_dir_root .. "/assets/",
			"{COPY} ./assets" .. " ../" .. output_dir_root
		}
		


	filter "configurations:Debug"
		defines  {"DEBUG"}
		runtime "Debug"
		symbols "On"
		optimize "Off"
		staticruntime "off"

	filter "configurations:Release"
		defines {"RELEASE"}
		runtime "Release"
		symbols "Off"
		optimize "On"
		staticruntime "off"
