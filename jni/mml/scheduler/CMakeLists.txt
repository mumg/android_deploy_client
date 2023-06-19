
set (mml_scheduler_include
	include/mmlIScheduler.h
)	

set (mml_scheduler_source
	source/mmlScheduler.cpp
	source/mmlScheduler.h
)	

SOURCE_GROUP("include" FILES ${mml_scheduler_include})
SOURCE_GROUP("source"  FILES ${mml_scheduler_source}) 

		
set (mml_scheduler_SRCS  
	${mml_scheduler_include}
	${mml_scheduler_source}
)
include_directories(
					${MML_CORE}/core/include 
					include 
					source
					)
mml_add_component(mml_scheduler) 

set_target_properties ( mml_scheduler PROPERTIES FOLDER core/mml/utils )
