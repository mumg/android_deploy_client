
set (mml_binary_serialization
	../include/mmlIDeserializer.h
	../include/mmlISerializer.h
)	


set (mml_binary_serialization_source
	mml_binary_serialization.cpp
	mmlBinaryDeserializer.cpp
	mmlBinaryDeserializer.h
	mmlBinarySerializer.cpp
	mmlBinarySerializer.h
)	

SOURCE_GROUP("include" FILES ${mml_binary_serialization_include})
SOURCE_GROUP("source"  FILES ${mml_binary_serialization_source}) 

		
set (mml_binary_serialization_SRCS
	${mml_binary_serialization_include}
	${mml_binary_serialization_source}
)

include_directories(
					${MML_CORE}/core/include
					../include
					)
mml_add_component(mml_binary_serialization)

set_target_properties ( mml_binary_serialization PROPERTIES FOLDER core/mml/serialization )