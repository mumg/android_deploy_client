gen_output()

include_directories(${CMAKE_CURRENT_BINARY_DIR})


add_executable(settings
               ../main.cpp
)

set_target_properties(settings PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${MML_BIN_DIR})