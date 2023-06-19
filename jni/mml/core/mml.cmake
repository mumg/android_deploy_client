
###########################
# mml_add_component TARGET_NAME
###########################
MACRO(mml_add_component TARGET_NAME)
	add_definitions(${MML_CORE_DEFS})
	include_directories(${MML_CORE_INCLUDE})
if(MSVC)
	add_definitions(-DMML_WIN)
	if(CMAKE_CL_64)
		add_definitions(-DMML_WIN64)
	else()
		add_definitions(-DMML_WIN32)
	endif()
else(MSVC)
        add_definitions(-DMML_POSIX)
endif(MSVC)
if(MML_SHARED)
	add_definitions(-DMML_SHARED)
	add_library (${TARGET_NAME} SHARED ${${TARGET_NAME}_SRCS})
    if(MSVC)
        set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${MML_BIN_DIR})
    else(MSVC)
		set_target_properties(${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MML_BIN_DIR})
    endif(MSVC)
		
else(MML_SHARED)
	add_library (${TARGET_NAME} ${${TARGET_NAME}_SRCS})
endif(MML_SHARED)
	target_link_libraries (${TARGET_NAME} mml_core ${${TARGET_NAME}_LIBS})	
	
if(MML_INSTALL_DIR)
	install(TARGETS ${TARGET_NAME}
		DESTINATION ${MML_INSTALL_DIR})
endif(MML_INSTALL_DIR)
ENDMACRO(mml_add_component)

###########################
# mml_add_component TARGET_NAME
###########################
MACRO(mml_add_library TARGET_NAME)
	add_definitions(${MML_CORE_DEFS})
	include_directories(${MML_CORE_INCLUDE})
if(MSVC)
	add_definitions(-DMML_WIN)
	if(CMAKE_CL_64)
		add_definitions(-DMML_WIN64)
	else()
		add_definitions(-DMML_WIN32)
	endif()
else(MSVC)
        add_definitions(-DMML_POSIX)
endif(MSVC)
	add_library (${TARGET_NAME} ${${TARGET_NAME}_SRCS})
ENDMACRO(mml_add_library)

########################
# mml_add_module(TARGET_NAME)
########################
MACRO(mml_add_module TARGET_NAME)
	add_definitions(${MML_CORE_DEFS})
	include_directories(${MML_CORE_INCLUDE})
if(MSVC)
	add_definitions(-DMML_WIN)
	if(CMAKE_CL_64)
		add_definitions(-DMML_WIN64)
	else()
		add_definitions(-DMML_WIN32)
	endif()
else(MSVC)
        add_definitions(-DMML_POSIX)
endif(MSVC)	
 	add_library (${TARGET_NAME} SHARED ${${TARGET_NAME}_SRCS})
    if(MSVC)
        set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${MML_BIN_DIR})
    else(MSVC)
		set_target_properties(${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MML_BIN_DIR})
    endif(MSVC)
if(MSVC)
	target_link_libraries (${TARGET_NAME} ${${TARGET_NAME}_LIBS} mml_core)	
else(MSVC)
	if(MML_ANDROID)
            if(CMAKE_STRIP)
                add_custom_command(TARGET ${TARGET_NAME} POST_BUILD 
                                   COMMAND ${CMAKE_STRIP} --strip-all $<TARGET_FILE:${TARGET_NAME}>)
            endif()
	ENDIF()
if(MML_ANDROID)
	target_link_libraries (${TARGET_NAME} ${${TARGET_NAME}_LIBS} mml_core)
else()
	target_link_libraries (${TARGET_NAME} ${${TARGET_NAME}_LIBS} mml_core pthread rt)
endif()
endif(MSVC)
ENDMACRO(mml_add_module)

########################
# mml_add_executable(TARGET_NAME)
########################
MACRO(mml_add_executable TARGET_NAME)
	add_definitions(${MML_CORE_DEFS})
	include_directories(${MML_CORE_INCLUDE})
if(MSVC)
	add_definitions(-DMML_WIN)
	if(CMAKE_CL_64)
		add_definitions(-DMML_WIN64)
	else()
		add_definitions(-DMML_WIN32)
	endif()
else(MSVC)
        add_definitions(-DMML_POSIX)
endif(MSVC)
if(MML_SHARED)
	add_definitions(-DMML_SHARED)
endif(MML_SHARED)
 	add_executable (${TARGET_NAME} ${${TARGET_NAME}_SRCS})
if(MSVC)
	target_link_libraries (${TARGET_NAME} ${${TARGET_NAME}_LIBS} mml_core)	
else(MSVC)
	if(MML_ANDROID)
            if(CMAKE_STRIP)
                add_custom_command(TARGET ${TARGET_NAME} POST_BUILD 
                                   COMMAND ${CMAKE_STRIP} --strip-all $<TARGET_FILE:${TARGET_NAME}>)
            endif()
	ENDIF()
if(NOT MML_ANDROID)
	target_link_libraries (${TARGET_NAME} ${${TARGET_NAME}_LIBS} mml_core pthread rt)	
else()
    target_link_libraries (${TARGET_NAME} ${${TARGET_NAME}_LIBS} mml_core)	
endif()
endif(MSVC)
	set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${MML_BIN_DIR})
if(MML_INSTALL_DIR)
	install(TARGETS ${TARGET_NAME}
		DESTINATION ${MML_INSTALL_DIR})
endif(MML_INSTALL_DIR)
		
ENDMACRO(mml_add_executable)

MACRO(mml_set_install_dir DIR_NAME)
set(MML_INSTALL_DIR ${DIR_NAME}) 
ENDMACRO(mml_set_install_dir)

include(CMakeParseArguments)

# Function to wrap a given string into multiple lines at the given column position.
# Parameters:
#   VARIABLE    - The name of the CMake variable holding the string.
#   AT_COLUMN   - The column position at which string will be wrapped.
function(WRAP_STRING)
    set(oneValueArgs VARIABLE AT_COLUMN)
    cmake_parse_arguments(WRAP_STRING "${options}" "${oneValueArgs}" "" ${ARGN})



    set(${WRAP_STRING_VARIABLE} "${lines}" PARENT_SCOPE)
endfunction()

# Function to embed contents of a file as byte array in C/C++ header file(.h). The header file
# will contain a byte array and integer variable holding the size of the array.
# Parameters
#   SOURCE_FILE     - The path of source file whose contents will be embedded in the header file.
#   VARIABLE_NAME   - The name of the variable for the byte array. The string "_SIZE" will be append
#                     to this name and will be used a variable name for size variable.
#   HEADER_FILE     - The path of header file.
#   APPEND          - If specified appends to the header file instead of overwriting it
#   NULL_TERMINATE  - If specified a null byte(zero) will be append to the byte array. This will be
#                     useful if the source file is a text file and we want to use the file contents
#                     as string. But the size variable holds size of the byte array without this
#                     null byte.
# Usage:
#   bin2h(SOURCE_FILE "Logo.png" HEADER_FILE "Logo.h" VARIABLE_NAME "LOGO_PNG")
function(BIN2H)
    set(options APPEND NULL_TERMINATE)
    set(oneValueArgs SOURCE_FILE VARIABLE_NAME HEADER_FILE)
    cmake_parse_arguments(BIN2H "${options}" "${oneValueArgs}" "" ${ARGN})
    # reads source file contents as hex string
	file(READ ${BIN2H_SOURCE_FILE} hexString HEX)
    string(LENGTH ${hexString} hexStringLength)
    string(LENGTH ${hexString} stringLength)
    math(EXPR offset "0")

    while(stringLength GREATER 0)

        if(stringLength GREATER 32)
            set(length 32)
        else()
            math(EXPR length "${stringLength}")
        endif()

        string(SUBSTRING ${hexString} ${offset} ${length} line)
        set(lines "${lines}\n${line}")

        math(EXPR stringLength "${stringLength} - ${length}")
        math(EXPR offset "${offset} + ${length}")
    endwhile()
	set ( hexString ${lines})
	
    math(EXPR arraySize "${hexStringLength} / 2")

    # adds '0x' prefix and comma suffix before and after every byte respectively
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " arrayValues ${hexString})
    # removes trailing comma
    string(REGEX REPLACE ", $" "" arrayValues ${arrayValues})

    # converts the variable name into proper C identifier
    string(MAKE_C_IDENTIFIER "${BIN2H_VARIABLE_NAME}" BIN2H_VARIABLE_NAME)
    string(TOLOWER "${BIN2H_VARIABLE_NAME}" BIN2H_VARIABLE_NAME)

    # declares byte array and the length variables
    set(arrayDefinition "const char ${BIN2H_VARIABLE_NAME}[] = { ${arrayValues} };")
    set(arraySizeDefinition "const int ${BIN2H_VARIABLE_NAME}_SIZE = ${arraySize};")

    set(declarations "${arrayDefinition}\n\n${arraySizeDefinition}\n\n")
    file(WRITE ${BIN2H_HEADER_FILE} "${declarations}")
endfunction()

function(string_remove_ending original ending)
  string(LENGTH "${ending}" len)
  string(LENGTH "${${original}}" orig_len)
  math(EXPR len "${orig_len} - ${len}")
  if(len LESS -1)
    set(len -1)
  endif()
  string(SUBSTRING "${${original}}" 0 ${len} result)
  set(${original} ${result} PARENT_SCOPE)
endfunction()
