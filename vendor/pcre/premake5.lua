project "pcre"
	language "C++"
	targetname "pcre3"
	
	defines { "HAVE_CONFIG_H" }
	includedirs { ".", "shipped_for_mta_win32" }
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources/*"] = {"**.c", "**.cc"},
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"pcre_chartables.c",
		"pcre_compile.c",
		"pcre_config.c",
		"pcre_dfa_exec.c",
		"pcre_exec.c",
		"pcre_fullinfo.c",
		"pcre_get.c",
		"pcre_globals.c",
		"pcre_info.c",
		"pcre_maketables.c",
		"pcre_newline.c",
		"pcre_ord2utf8.c",
		"pcre_refcount.c",
		"pcre_scanner.cc",
		"pcre_stringpiece.cc",
		"pcre_study.c",
		"pcre_tables.c",
		"pcre_try_flipped.c",
		"pcre_ucd.c",
		"pcre_valid_utf8.c",
		"pcre_version.c",
		"pcre_xclass.c",
		"pcrecpp.cc",
		"pcreposix.c",
		"shipped_for_mta_win32/config.h",
		"pcre.h",
		"pcre_internal.h",
		"pcre_scanner.h",
		"pcre_stringpiece.h",
		"pcrecpp.h",
		"pcrecpp_internal.h",
		"pcrecpparg.h",
		"pcreposix.h",
		"ucp.h"
	}
	
	filter "system:windows"
		kind "SharedLib"
		targetdir(buildpath("server/mods/deathmatch"))
		
	filter {"system:windows", "platforms:x86"}
		postbuildcommands {
			copy "mods/deathmatch"
		}

	filter {"system:windows", "platforms:x64"}
		targetdir(buildpath("server/x64"))

	filter "system:not windows"
		kind "StaticLib"

	disablewarnings { "4251" }
	filter {"system:windows"}
		linkoptions { "/ignore:4217", "/ignore:4049" }
