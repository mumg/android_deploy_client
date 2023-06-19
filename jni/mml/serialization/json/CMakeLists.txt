
set (mml_json_serialization
	../include/mmlIDeserializer.h
	../include/mmlISerializer.h
)	


set (mml_json_serialization_source
	mml_json_serialization.cpp
	mmlJSONDeserializer.cpp
	mmlJSONDeserializer.h
	mmlJSONSerializer.cpp
	mmlJSONSerializer.h
)	

SOURCE_GROUP("include" FILES ${mml_json_serialization_include})
SOURCE_GROUP("source"  FILES ${mml_json_serialization_source}) 

		
set (mml_json_serialization_SRCS
	${mml_json_serialization_include}
	${mml_json_serialization_source}
)

include_directories(
					${MML_CORE}/core/include
					../include
					)
mml_add_component(mml_json_serialization)

set_target_properties ( mml_json_serialization PROPERTIES FOLDER core/mml/serialization )