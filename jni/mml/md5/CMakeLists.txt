set (mml_md5_source
	source/mml_md5.cpp
	source/mmlMD5OutputStream.cpp
	source/mmlMD5OutputStream.h
	source/mmlMD5InputStream.cpp
	source/mmlMD5InputStream.h
	md5/md5.c
	include/mml_md5_uuids.h
)	

SOURCE_GROUP("source"  FILES ${mml_md5_source}) 

		
set (mml_md5_SRCS  
	${mml_md5_include}
	${mml_md5_source}
)

include_directories(
                    ${MML_CORE}/core/include
					include 
					source
					md5
					)
mml_add_component(mml_md5) 

set_target_properties ( mml_md5 PROPERTIES FOLDER core/mml )