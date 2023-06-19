set (mml_bridge_include
	include/mmlIBridgeStreamServer.h
)

set (mml_bridge_source
	source/mml_bridge.cpp
	source/mmlBridgeStreamServer.cpp
	source/mmlBridgeStreamServer.h
)	

SOURCE_GROUP("source"  FILES ${mml_bridge_source}) 

		
set (mml_bridge_SRCS  
	${mml_bridge_include}
	${mml_bridge_source}
)

include_directories(
                    ${MML_CORE}/core/include
					include 
					source
					)
mml_add_component(mml_bridge) 

set_target_properties ( mml_bridge PROPERTIES FOLDER core/mml )
