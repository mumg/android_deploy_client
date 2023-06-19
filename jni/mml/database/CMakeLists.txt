set (mml_database_include 
	include/mmlIDatabase.h
)	


set (mml_database_source
	source/mmlDatabase.h
	source/mmlDatabase.cpp
	source/mml_database.cpp
)	

SOURCE_GROUP("include" FILES ${mml_database_include})
SOURCE_GROUP("source"  FILES ${mml_database_source}) 

		
set (mml_database_SRCS
	${mml_database_include}
	${mml_database_source}
)

include_directories(
					${MML_CORE}/core/include
					include
					source
					${MML_CORE}/thirdparty/sqlite3
					)

mml_add_component(mml_database)

set_target_properties ( mml_database PROPERTIES FOLDER core/mml )
