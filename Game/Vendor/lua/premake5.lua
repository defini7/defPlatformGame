---@diagnostic disable: undefined-global
project "lua"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "On"
    warnings "Off"

    targetdir ("%{wks.location}/Build/Target/" .. OUTPUT_DIR .. "/%{prj.name}")
    objdir ("%{wks.location}/Build/Obj/" .. OUTPUT_DIR .. "/%{prj.name}")

    files { "src/*" }
    
    removefiles
    {
      "src/lua.c",
      "src/onelua.c" 
    }

    filter "system:linux"
        pic "On"
        systemversion "latest"

    filter "system:macosx"
        pic "On"

    filter "system:windows"
		systemversion "latest"

    filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "speed"
