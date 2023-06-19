set (mml_crc32_source
	source/mml_crc32.cpp
	source/mmlCRC32OutputStream.cpp
	source/mmlCRC32OutputStream.h
	source/mmlCRC32InputStream.cpp
	source/mmlCRC32InputStream.h
	include/mml_crc32_uuids.h
)	

SOURCE_GROUP("source"  FILES ${mml_crc32_source}) 

		
set (mml_crc32_SRCS  
	${mml_crc32_include}
	${mml_crc32_source}
)

include_directories(
                    ${MML_CORE}/core/include
					include 
					source
					)
mml_add_component(mml_crc32) 

set_target_properties ( mml_crc32 PROPERTIES FOLDER core/mml )