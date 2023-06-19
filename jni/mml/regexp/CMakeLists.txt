add_subdirectory(pcre)

set (mml_regexp_include 
	include/mmlIRegexp.h
)	


set (mml_regexp_source
	source/mmlRegexp.h
	source/mmlRegexp.cpp
	source/mml_regexp.cpp
)	

SOURCE_GROUP("include" FILES ${mml_regexp_include})
SOURCE_GROUP("source"  FILES ${mml_regexp_source}) 

		
set (mml_regexp_SRCS
	${mml_regexp_include}
	${mml_regexp_source}
)

include_directories(
					${MML_CORE}/core/include
					include
					source
					pcre
					${PROJECT_BINARY_DIR}/pcre
					)

set(mml_regexp_LIBS
	pcrecpp
	pcre)

mml_add_component(mml_regexp)

set_target_properties ( mml_regexp PROPERTIES FOLDER core/mml )