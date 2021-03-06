project "CEF"
	targetname "CEF"
	language "C++"
	kind "StaticLib"

	includedirs { "." }
	defines { "V8_DEPRECATION_WARNINGS", "WIN32", "_WINDOWS", "NOMINMAX", "PSAPI_VERSION=1", "_CRT_RAND_S", "CERT_CHAIN_PARA_HAS_EXTRA_FIELDS", "WIN32_LEAN_AND_MEAN",
		"_ATL_NO_OPENGL", "_HAS_EXCEPTIONS=0", "_SECURE_ATL", "CHROMIUM_BUILD", "TOOLKIT_VIEWS=1", "USE_AURA=1", "USE_ASH=1", "USE_DEFAULT_RENDER_THEME=1",
		"USE_LIBJPEG_TURBO=1", "ENABLE_ONE_CLICK_SIGNIN", "ENABLE_PRE_SYNC_BACKUP", "ENABLE_REMOTING=1", "ENABLE_WEBRTC=1", "ENABLE_PEPPER_CDMS", "ENABLE_CONFIGURATION_POLICY",
		"ENABLE_NOTIFICATIONS", "ENABLE_HIDPI=1", "ENABLE_EGLIMAGE=1", "__STD_C", "_CRT_SECURE_NO_DEPRECATE", "_SCL_SECURE_NO_DEPRECATE", "NTDDI_VERSION=0x06020000",
		"_USING_V110_SDK71_", "ENABLE_TASK_MANAGER=1", "ENABLE_EXTENSIONS=1", "ENABLE_PLUGIN_INSTALLATION=1", "ENABLE_PLUGINS=1", "ENABLE_SESSION_SERVICE=1",
		"ENABLE_THEMES=1", "ENABLE_AUTOFILL_DIALOG=1", "ENABLE_BACKGROUND=1", "ENABLE_GOOGLE_NOW=1", "CLD_VERSION=2", "CLD2_DATA_SOURCE=static", "ENABLE_FULL_PRINTING=1",
		"ENABLE_PRINTING=1", "ENABLE_SPELLCHECK=1", "ENABLE_CAPTIVE_PORTAL_DETECTION=1", "ENABLE_APP_LIST=1", "ENABLE_SETTINGS_APP=1", "ENABLE_MANAGED_USERS=1",
		"ENABLE_MDNS=1", "ENABLE_SERVICE_DISCOVERY=1", "ENABLE_WIFI_BOOTSTRAPPING=1", "ENABLE_LOAD_COMPLETION_HACKS=1", "USING_CEF_SHARED", "__STDC_CONSTANT_MACROS",
		"__STDC_FORMAT_MACROS", "DYNAMIC_ANNOTATIONS_ENABLED=1", "WTF_USE_DYNAMIC_ANNOTATIONS=1" }
	
	files {
		"premake5.lua",
		"**.cc",
		"**.h",
	}

	local path = buildpath(".")
	local cef_path = "%{wks.location}/../Vendor/cef3/"
	postbuildcommands {
		"{COPY} "..cef_path.."Release/* "..path.."mta",
		"{COPY} "..cef_path.."Resources/icudtl.dat "..path.."mta",
		"{COPY} "..cef_path.."Resources/*.pak "..path.."mta/cef",
		"{COPY} "..cef_path.."Resources/locales/* "..path.."mta/cef/locales"
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" } 
	filter "system:not windows"
		flags { "ExcludeFromBuild" } 
