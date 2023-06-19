if(MML_SSL)
add_definitions(-DMML_SSL)
endif()


set (mml_network_include
	include/mmlINetworkStreamServer.h
	include/mmlINetworkStreamServerSSL.h
	include/mmlINetworkStreamClient.h
	include/mmlINetworkStreamClientSSL.h
)

set (mml_network_source
	source/mml_network.cpp
	source/mmlNetworkCommon.cpp
	source/mmlNetworkCommon.h
	source/mmlNetworkStreamServer.cpp
	source/mmlNetworkStreamServer.h
	source/mmlNetworkStreamClient.cpp
	source/mmlNetworkStreamClient.h
)

if(MML_SSL)
set (mml_network_source
	${mml_network_source}
        #source/mmlNetworkStreamServerSSL.cpp
        #source/mmlNetworkStreamServerSSL.h
        source/mmlNetworkStreamClientSSL.cpp
        source/mmlNetworkStreamClientSSL.h
)

endif()	

SOURCE_GROUP("include" FILES ${mml_network_include})
SOURCE_GROUP("source"  FILES ${mml_network_source}) 

		
set (mml_network_SRCS
	${mml_network_include}
	${mml_network_source}
)
include_directories(../thirdparty/mbedtls/include)
if(MML_SSL)
set(mml_network_LIBS mbedcrypto mbedtls mbedx509)
endif()

include_directories(
					${MML_CORE}/core/include
					include
					source
					)
mml_add_component(mml_network)

set_target_properties ( mml_network PROPERTIES FOLDER core/mml )
