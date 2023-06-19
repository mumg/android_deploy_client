
set (mml_xml_include 
	include/mmlIXMLDocument.h
	include/mmlIXMLNode.h
)	


set (mml_xml_source
	source/mml_xml_component.cpp
	source/mmlXMLDocument.cpp
	source/mmlXMLDocument.h
	source/mmlXMLNode.cpp
	source/mmlXMLNode.h
	source/mmlXMLPath.cpp
	source/mmlXMLPath.h
)	

SOURCE_GROUP("include" FILES ${mml_xml_include})
SOURCE_GROUP("source"  FILES ${mml_xml_source}) 

		
set (mml_xml_SRCS
	${mml_xml_include}
	${mml_xml_source}
)

include_directories(
					${MML_CORE}/core/include
					include
					source
					)
mml_add_component(mml_xml)

set_target_properties ( mml_xml PROPERTIES FOLDER core/mml )