workspace "microECS"
    configurations { "Debug", "Release" }
    architecture "x64"
    startproject "microECS"
    flags { "MultiProcessorCompile" }

project "microECS"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin-int/%{cfg.buildcfg}"

    files {
        "include/**.h",
        "src/**.cpp",
    }

    includedirs {
        "include",
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "MECS_PLATFORM_WINDOWS",
        }

    filter "configurations:Debug"
        defines "MECS_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "MECS_RELEASE"
        optimize "On"