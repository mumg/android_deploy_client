
add_library(libsqlite3
	sqlite3.c
	sqlite3.h
)

set_target_properties ( libsqlite3 PROPERTIES FOLDER core/mml/thirdparty )

add_executable(sqlite3
	shell.c
)

target_link_libraries ( sqlite3 libsqlite3 )

if(UNIX AND NOT MML_ANDROID)
	target_link_libraries ( sqlite3 dl pthread )
endif()

set_target_properties ( sqlite3 PROPERTIES FOLDER core/mml/thirdparty )