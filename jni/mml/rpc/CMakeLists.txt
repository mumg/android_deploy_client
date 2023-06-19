set (mml_rpc_include
	include/mmlIRCPServer.h
)	

set (mml_rpc_source
	source/mml_rpc.cpp
	source/mmlRPCServer.cpp
	source/mmlRPCServer.h
)	

SOURCE_GROUP("include" FILES ${mml_rpc_include})
SOURCE_GROUP("source"  FILES ${mml_rpc_source}) 

		
set (mml_rpc_SRCS  
	${mml_rpc_include}
	${mml_rpc_source}
)


include_directories(
                    ${MML_CORE}/core/include
					${MML_CORE}/crc/include
					${MML_CORE}/serialization/include
					include 
					source
					)
mml_add_component(mml_rpc) 

set_target_properties ( mml_rpc PROPERTIES FOLDER core/mml )
