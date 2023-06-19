gen_output()

include_directories(${CMAKE_CURRENT_BINARY_DIR})


add_executable(service
               ../main.cpp
)

set_target_properties(service PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${MML_BIN_DIR})