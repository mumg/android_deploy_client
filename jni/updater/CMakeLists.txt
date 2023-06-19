execute_process(COMMAND git log --pretty=format:%h -n 1 HEAD OUTPUT_VARIABLE GIT_VER)
message("GIT version is ${GIT_VER}")

add_definitions(-DGIT_VERSION=\"${GIT_VER}\")

set (android_updater_module_sources
	android_cmd_dispatcher.cpp
	android_cmd_dispatcher.h
	android_cmd_logs.cpp
	android_cmd_shell.cpp
	android_cmd_update.cpp
	android_updater_module.cpp
	android_config.cpp
	android_http_fetch.cpp
	android_http_fetch.h
	android_properties.cpp
	android_properties.h
	android_utils.cpp
	android_utils.h
	android_updater_subsystem.cpp
	android_updater_database.cpp
	android_updater_database.h
	android_http_update.cpp
	android_http_update.h
	android_updater_task_tracking.cpp
	android_updater_task_reboot.cpp
	android_updater_task_cleanup.cpp
	android_updater_task_check.cpp
	android_settings.cpp
	android_settings.h
	android_reboot.h
)

SOURCE_GROUP("" FILES ${android_updater_module_sources}) 

set (android_updater_module_SRCS
	${android_updater_module_sources}
)

set (android_updater_module_LIBS
	mml_core
	mml_gzip
	mml_http
	mml_network
	mml_process
	mml_json_serialization
	mml_binary_serialization
	mml_crc32
	mml_md5
	mml_regexp
	mml_sha1
	mml_websocket
	mml_base64
	mml_database
	mml_scheduler
	libsqlite3
)


if(NOT MSVC AND NOT MML_ANDROID)
set (android_updater_module_LIBS
    ${android_updater_module_LIBS}
    uuid
    dl
)
endif()

if(MML_SSL)
set (android_updater_module_LIBS
    ${android_updater_module_LIBS}
    mbedcrypto
	mbedtls
	mbedx509
)
endif()

include_directories(
					${MML_CORE}/core/include
					${MML_CORE}/http/include 
					${MML_CORE}/network/include 
					${MML_CORE}/md5/include
					${MML_CORE}/serialization/include
					${MML_CORE}/regexp/include
					${MML_CORE}/process/include
					${MML_CORE}/websocket/include
					${MML_CORE}/database/include
					${MML_CORE}/scheduler/include
					)

mml_add_module(android_updater_module)
