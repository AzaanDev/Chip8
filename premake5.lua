workspace "Chip8"
	architecture "x64"
	configurations { "Debug", "Release" }
	startproject "Chip8"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["SDL2"] = "%{wks.location}/vendor/SDL2/SDL2-2.24.1/include"
IncludeDir["Chip8"] = "%{wks.location}/Chip8/include"


include "Chip8"