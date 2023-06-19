set (mml_process_include 
	include/mmlIProcess.h
)	


set (mml_process_source
	source/mmlProcess.h
	source/mmlProcess.cpp
	source/mml_process.cpp
)	

SOURCE_GROUP("include" FILES ${mml_process_include})
SOURCE_GROUP("source"  FILES ${mml_process_source}) 

		
set (mml_process_SRCS
	${mml_process_include}
	${mml_process_source}
)

include_directories(
					${MML_CORE}/core/include
					include
					source
					)

mml_add_component(mml_process)

set_target_properties ( mml_process PROPERTIES FOLDER core/mml )
