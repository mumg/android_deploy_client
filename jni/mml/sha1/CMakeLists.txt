set (mml_sha1_source
	source/mml_sha1.cpp
	source/mmlSHA1OutputStream.cpp
	source/mmlSHA1OutputStream.h
	source/mmlSHA1InputStream.cpp
	source/mmlSHA1InputStream.h
	sha1/sha1.c
	include/mml_sha1_uuids.h
)	

SOURCE_GROUP("source"  FILES ${mml_sha1_source}) 

		
set (mml_sha1_SRCS  
	${mml_sha1_include}
	${mml_sha1_source}
)

include_directories(
                    ${MML_CORE}/core/include
					include 
					source
					sha1
					)
mml_add_component(mml_sha1) 

set_target_properties ( mml_sha1 PROPERTIES FOLDER core/mml )