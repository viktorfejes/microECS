workspace "microECSTests"
    configurations { "Debug", "Release" }

project "microECSTests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    includedirs {
        "../microECS",
        "dependencies",
    }

    files {
        "./*.cpp",
    }

    filter "configurations:Debug"
        defines "ENGINE_DEBUG"
        runtime "Debug"
        symbols "on"
    
    filter "configurations:Release"
        defines "ENGINE_RELEASE"
        runtime "Release"
        optimize "on"
