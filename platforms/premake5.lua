dofile "premake-utils.lua"

solution "corewar"
	configurations {"Debug", "Release"}
	location ("../build")

project "vm"
	kind "ConsoleApp"
	language "C"
	files {
		"../vm/**.c",
		"../vm/**.h",
		"../vm/shaders/*.frag",
		"../vm/shaders/*.vert",
		"../vm/Externals/glew/src/*.c",
		"../vm/Externals/stb/*.h"
	}
	includedirs {"../vm/Externals/glfw/include",
	"../vm/Externals/glew/include",
	"../vm/Externals/glm"}

	floatingpoint "Fast"
	flags       { "Symbols" }
	debugdir "../vm"
	location ("../build")

	os.mkdir("../build")
	os.copydir("../vm/shaders", "../build/shaders")

	defines { "GLEW_STATIC", "GLM_FORCE_RADIANS" }

	configuration "vs*"
		defines     { "_CRT_SECURE_NO_WARNINGS" }

	configuration "vs2005"
		defines        {"_CRT_SECURE_NO_DEPRECATE" }

	configuration "windows"
		os.copyfile("../vm/Externals/glfw/lib/glfw3.dll", "../build/glfw3.dll")
		libdirs "../vm/Externals/glfw/lib"
		system "Windows"
		links {"opengl32", "glfw3dll", "Ws2_32"}

	configuration { "macosx", "gmake" }
		toolset "clang"
		buildoptions { "-mmacosx-version-min=10.4" }
		linkoptions  { "-mmacosx-version-min=10.4" }

	configuration "macosx"
		platforms 	{"Universal"}
		libdirs 	"../vm/Externals/glfw/lib/osx"
		links       { "glfw3", "OpenGL.framework", "CoreFoundation.framework", "CoreVideo.framework", "Cocoa.framework", "IOKit.framework", "AGL.framework"}

	configuration "linux"
		if os.is64bit() then
			libdirs "../vm/Externals/glfw/lib/linux/x86_64"
		else
			libdirs "../vm/Externals/glfw/lib/linux/x86"
		end
		links       { "glfw3", "X11", "Xxf86vm", "Xrandr", "Xi", "GL", "pthread"}

	configuration "Debug"
		defines { "DEBUG" }
		optimize "Debug"

	configuration "Release"
		defines { "NDEBUG" }
		optimize "Full"
