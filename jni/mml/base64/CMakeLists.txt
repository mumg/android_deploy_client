
set (mml_base64_source
	source/mml_base64.cpp
	source/mmlBase64OutputStream.cpp
	source/mmlBase64OutputStream.h
	source/mmlBase64InputStream.cpp
	source/mmlBase64InputStream.h
	include/mml_base64_uuids.h
)	

SOURCE_GROUP("source"  FILES ${mml_base64_source}) 

		
set (mml_base64_SRCS  
	${mml_base64_source}
)

include_directories(
                    ${MML_CORE}/core/include
                    ${MML_CORE}/network/include
					include 
					source
					)
mml_add_component(mml_base64) 

set_target_properties ( mml_base64 PROPERTIES FOLDER core/mml )