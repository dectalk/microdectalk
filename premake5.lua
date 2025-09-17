flags("MultiProcessorCompile")

workspace("DECTalk")
        configurations({
                "Debug",
                "Release"
        })
        platforms({
                "Native",
		"Win32",
		"Win64"
        })
        defaultplatform("Native")

newoption({
        trigger = "build-speak",
        value = "type",
        description = "Build speak or not",
        allowed = {
		{"yes", "Build speak"},
		{"no", "Do not build speak"}
        },
        category = "DECTalk",
        default = "no"
})

filter("platforms:Win32")
        system("windows")
        architecture("x86")
        gccprefix("i686-w64-mingw32-")

filter("platforms:Win64")
        system("windows")
        architecture("x86_64")
        gccprefix("x86_64-w64-mingw32-")

filter({})

project("libdtc")
	kind("StaticLib")
	language("C")
	targetname("dtc")
	includedirs("include")
	files("src/*.c")
	removefiles("src/main.c")
	characterset("MBCS")
	filter("configurations:Debug")
		defines({"DICDEBUG", "DEBUG"})
		symbols("On")
	filter("configurations:Release")
		optimize("On")
	filter({})
	defines({
		"_REENTRANT",
		"NOMME",
		"LTSSIM",
		"TTSSIM",
		"ANSI",
		"BLD_DECTALK_DLL",
		"ENGLISH",
		"ENGLISH_US",
		"ACCESS32",
		"TYPING_MODE",
		"ACNA",
		"DISABLE_AUDIO",
		"SINGLE_THREADED",
		"__inline="
	})
	filter("system:windows")
		files("mman-win32/*.c")
		defines("__unix__")
	filter({})

project("say")
	kind("ConsoleApp")
	language("C")
	includedirs("include")
	files("src/main.c")
	characterset("MBCS")
	links({
		"libdtc",
		"m"
	})

if _OPTIONS["build-speak"] == "yes" then
	project("speak")
		kind("WindowedApp")
		language("C")
		includedirs("include")
		characterset("MBCS")
		includedirs("speak-common")
		files("speak-common/*.c")
		filter("system:not windows")
			files("speak/*.c")
			includedirs({
				"/usr/X11R7/include",
				"/usr/pkg/include"
			})
			libdirs({
				"/usr/X11R7/lib",
				"/usr/pkg/lib"
			})
		filter("system:windows")
			files("speak-w32/*.c")
			files("speak-w32/*.rc")
		filter({})
		links("libdtc")

		filter("system:not windows")
			links({
				"Xpm",
				"Xm",
				"Xt"
			})
			links("m")
		filter("system:windows")
			links({
				"gdi32",
				"comctl32"
			})
		filter({})
end
