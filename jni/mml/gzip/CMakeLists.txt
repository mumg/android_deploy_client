add_subdirectory(source/zlib)

set (mml_gzip_include
	include/mmlIGZIPOutputStream.h
	include/mmlIGZIPInputStream.h
)	

set (mml_gzip_source
	source/mml_gzip.cpp
	source/mmlGZIPOutputStream.cpp
	source/mmlGZIPOutputStream.h
	source/mmlGZIPInputStream.cpp
	source/mmlGZIPInputStream.h
)	

SOURCE_GROUP("include" FILES ${mml_gzip_include})
SOURCE_GROUP("source"  FILES ${mml_gzip_source}) 

		
set (mml_gzip_SRCS  
	${mml_gzip_include}
	${mml_gzip_source}
)

set (mml_gzip_LIBS
	zlib
)

include_directories(
                    ${MML_CORE}/core/include
                    ${MML_CORE}/network/include
					include 
					source
					source/zlib
					)
mml_add_component(mml_gzip) 

set_target_properties ( mml_gzip PROPERTIES FOLDER core/mml )