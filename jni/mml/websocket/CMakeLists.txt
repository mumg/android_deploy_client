set (mml_network_include
	include/mmlIWebSocketClient.h
)

set (mml_websocket_source
	source/mml_websocket.cpp
	source/mmlWebSocketClient.cpp
	source/mmlWebSocketClient.h
)

SOURCE_GROUP("include" FILES ${mml_websocket_include})
SOURCE_GROUP("source"  FILES ${mml_websocket_source}) 

		
set (mml_websocket_SRCS
	${mml_websocket_include}
	${mml_websocket_source}
)

include_directories(
					${MML_CORE}/core/include
					${MML_CORE}/http/include
					${MML_CORE}/base64/include
					${MML_CORE}/sha1/include
					${MML_CORE}/serialization/include
					include
					source
					)
mml_add_component(mml_websocket)

set_target_properties ( mml_websocket PROPERTIES FOLDER core/mml )
