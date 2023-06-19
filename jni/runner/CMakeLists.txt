set (android_updater_sources
	android_updater.c
)

SOURCE_GROUP("" FILES ${android_updater_sources}) 

set (android_updater_SRCS
	${android_updater_sources}
)


if(NOT MSVC AND NOT MML_ANDROID)
set (android_updater_LIBS
    uuid
    dl
)
endif()

mml_add_executable(android_updater)
