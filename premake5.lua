flags("MultiProcessorCompile")

workspace("DECTalk")
        configurations({
                "Debug",
                "Release"
        })
        platforms({
                "Native"
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

project("libdtc")
	kind("StaticLib")
	targetname("dtc")
	includedirs("include")
	files("src/*.c")
	removefiles("src/main.c")
	characterset("MBCS")
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
		"OS_SIXTY_FOUR_BIT",
		"ACNA",
		"DISABLE_AUDIO",
		"SINGLE_THREADED",
		"DICDEBUG",
		"__inline="
	})

project("say")
	kind("ConsoleApp")
	includedirs("include")
	files("src/main.c")
	characterset("MBCS")
	links({
		"libdtc",
		"m"
	})

if _OPTIONS["build-speak"] == "yes" then
	project("speak")
		kind("ConsoleApp")
		includedirs("include")
		files("speak/*.c")
		characterset("MBCS")
		includedirs({
			"/usr/X11R7/include",
			"/usr/pkg/include"
		})
		libdirs({
			"/usr/X11R7/lib",
			"/usr/pkg/lib"
		})
		links({
			"libdtc",
			"m",
			"Xpm",
			"Xm",
			"Xt"
		})
end
