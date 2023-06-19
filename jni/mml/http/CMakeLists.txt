set (mml_http_include
	include/mmlIHTTPServer.h
	include/mmlIHTTPClient.h
	include/mmlIHTTPRequest.h
	include/mmlIHTTPResponse.h
)	

set (mml_http_source
	source/mml_http.cpp
	source/mmlHTTPRequest.cpp
	source/mmlHTTPRequest.h
	source/mmlHTTPResponse.cpp
	source/mmlHTTPResponse.h
	source/mmlHTTPServer.cpp
	source/mmlHTTPServer.h
	source/mmlHTTPClient.cpp
	source/mmlHTTPClient.h
)	

SOURCE_GROUP("include" FILES ${mml_http_include})
SOURCE_GROUP("source"  FILES ${mml_http_source}) 

		
set (mml_http_SRCS  
	${mml_http_include}
	${mml_http_source}
)
include_directories(
                    ${MML_CORE}/core/include
                    ${MML_CORE}/network/include
					include 
					source
					)
mml_add_component(mml_http) 

set_target_properties ( mml_http PROPERTIES FOLDER core/mml )