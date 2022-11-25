-- premake5.lua
workspace "SybioteVision"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "SybioteVision"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "SybioteVision"