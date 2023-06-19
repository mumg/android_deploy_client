set (core_SRCS
	include/mmlICondition.h
	include/mmlICString.h
	include/mmlICStringStream.h
	include/mmlICStringUtils.h
	include/mmlIEvent.h
	include/mmlIList.h
	include/mmlILogger.h
	include/mmlIMutex.h
	include/mmlIObject.h
	include/mmlIObjectQueue.h
	include/mmlIProperties.h
	include/mmlIPerformanceCounter.h
	include/mmlIRandom.h
	include/mmlISleep.h
	include/mmlIThread.h
	include/mmlITimer.h
	include/mmlPtr.h
	include/mml_atomic.h
	include/mml_time.h
	include/mml_components.h
	include/mml_core.h
	include/mml_library.h
	include/mml_memory.h
	include/mml_services.h
	include/mml_strutils.h
	include/mmlIVariant.h
	include/mmlIFileSystem.h
    include/mmlIMemoryStream.h
    include/mmlIStream.h
    include/mmlIBuffer.h
	include/mmlICommandLine.h
	include/mmlIService.h
	include/mmlIStateMachine.h
	include/mmlIStreamHash.h
	include/mmlIMemoryBufferedQueue.h
	sources/mmlCString.cpp
	sources/mmlCString.h
	sources/mmlCStringStream.cpp
	sources/mmlCStringStream.h
	sources/mmlCStringUtils.cpp
	sources/mmlCStringUtils.h
	sources/mmlList.cpp
	sources/mmlList.h
	sources/mmlLogger.cpp
	sources/mmlLogger.h
	sources/mml_components.cpp
	sources/mml_library.cpp
	sources/mml_memory.cpp
	sources/mml_services.cpp
	sources/mml_strutils.cpp
	sources/mmlVariant.cpp
	sources/mmlVariant.h
	sources/mml_time_common.cpp
    sources/mmlMemoryStream.cpp
	sources/mmlMemoryStream.h
	sources/mmlBuffer.cpp
	sources/mmlCommandLine.cpp
	sources/mmlCommandLine.h
	sources/mmlFileSystem.cpp
	sources/mmlFileSystem.h
	sources/mmlStateMachine.cpp
	sources/mmlStateMachine.h
	sources/mml_config.cpp
	sources/mmlBufferedStream.cpp
	sources/mmlBufferedStream.h
	sources/mmlStreamCopy.cpp
	sources/mmlStreamCopy.h
	sources/mmlMemoryBufferedQueue.cpp
	sources/mmlMemoryBufferedQueue.h
	sources/mmlObjectQueue.cpp
	sources/mmlObjectQueue.h
)

if(MSVC)
set (core_SRCS ${core_SRCS}
	sources/portable/win32/mmlCondition.cpp
	sources/portable/win32/mmlCondition.h
	sources/portable/win32/mmlMutex.cpp
	sources/portable/win32/mmlMutex.h
	sources/portable/win32/mmlRandom.cpp
	sources/portable/win32/mmlRandom.h
	sources/portable/win32/mmlSleep.cpp
	sources/portable/win32/mmlSleep.h
	sources/portable/win32/mmlThread.cpp
	sources/portable/win32/mmlThread.h
	sources/portable/win32/mml_time.cpp
	sources/portable/win32/mmlTimer.cpp
	sources/portable/win32/mmlTimer.h
	sources/portable/win32/mmlDirectory.cpp
	sources/portable/win32/mmlDirectory.h
	sources/portable/win32/mmlFileSystemOS.cpp
	sources/portable/win32/mmlFileSystemOS.h
	sources/portable/win32/mmlPerformanceCounter.cpp
	sources/portable/win32/mmlPerformanceCounter.h
	sources/portable/win32/mml_atomic.cpp
)
if(MML_SHARED)
set (core_SRCS ${core_SRCS}
	sources/portable/win32/mmlComponentLoader.cpp
	sources/portable/win32/mmlComponentLoader.h
	)
endif(MML_SHARED)
else(MSVC)
set (core_SRCS ${core_SRCS}
	sources/portable/posix/mmlCondition.cpp
	sources/portable/posix/mmlCondition.h
	sources/portable/posix/mmlMutex.cpp
	sources/portable/posix/mmlMutex.h
	sources/portable/posix/mmlRandom.cpp
	sources/portable/posix/mmlRandom.h
	sources/portable/posix/mmlSleep.cpp
	sources/portable/posix/mmlSleep.h
	sources/portable/posix/mmlThread.cpp
	sources/portable/posix/mmlThread.h
	sources/portable/posix/mml_time.cpp
	sources/portable/posix/mmlTimer.cpp
	sources/portable/posix/mmlTimer.h
	sources/portable/posix/mmlDirectory.cpp
	sources/portable/posix/mmlDirectory.h
    sources/portable/posix/mmlFileSystemOS.cpp
    sources/portable/posix/mmlFileSystemOS.h
	sources/portable/posix/mmlPerformanceCounter.cpp
	sources/portable/posix/mmlPerformanceCounter.h	
	sources/portable/posix/mml_atomic.cpp
)
if(MML_SHARED)
set (core_SRCS ${core_SRCS}
	sources/portable/posix/mmlComponentLoader.cpp
	sources/portable/posix/mmlComponentLoader.h
	)
endif(MML_SHARED)
endif(MSVC)
include_directories(include sources)

if(MSVC)
add_definitions(-DMML_WIN)
include_directories(sources/portable/win32)
else(MSVC)
add_definitions(-DMML_POSIX)
include_directories(sources/portable/posix)
endif(MSVC)
if(MML_SHARED)
add_definitions(-DMML_SHARED -DMML_CORE)
add_library (mml_core SHARED ${core_SRCS})
if(MSVC)
        set_target_properties(mml_core PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${MML_BIN_DIR})
else(MSVC)
	set_target_properties(mml_core PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MML_BIN_DIR})
endif(MSVC)
if(MSVC)
target_link_libraries (mml_core)
else(MSVC)
if(MML_ANDROID)
target_link_libraries (mml_core ${CMAKE_DL_LIBS} pthread rt)
else()
target_link_libraries (mml_core ${CMAKE_DL_LIBS} uuid pthread rt)
endif()
endif(MSVC)
else(MML_SHARED)
add_library (mml_core ${core_SRCS})
endif(MML_SHARED)

if(MML_INSTALL_DIR)
	install(TARGETS mml_core
		DESTINATION ${MML_INSTALL_DIR})
endif(MML_INSTALL_DIR)

set_target_properties ( mml_core PROPERTIES FOLDER core/mml )
