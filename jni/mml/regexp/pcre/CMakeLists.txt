
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)


INCLUDE(CheckIncludeFile)
INCLUDE(CheckIncludeFileCXX)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckTypeSize)

CHECK_INCLUDE_FILE(dirent.h     HAVE_DIRENT_H)
CHECK_INCLUDE_FILE(stdint.h     HAVE_STDINT_H)
CHECK_INCLUDE_FILE(inttypes.h   HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE(sys/stat.h   HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE(sys/types.h  HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE(unistd.h     HAVE_UNISTD_H)
CHECK_INCLUDE_FILE(windows.h    HAVE_WINDOWS_H)

CHECK_INCLUDE_FILE_CXX(type_traits.h            HAVE_TYPE_TRAITS_H)
CHECK_INCLUDE_FILE_CXX(bits/type_traits.h       HAVE_BITS_TYPE_TRAITS_H)

CHECK_FUNCTION_EXISTS(bcopy     HAVE_BCOPY)
CHECK_FUNCTION_EXISTS(memmove   HAVE_MEMMOVE)
CHECK_FUNCTION_EXISTS(strerror  HAVE_STRERROR)
CHECK_FUNCTION_EXISTS(strtoll   HAVE_STRTOLL)
CHECK_FUNCTION_EXISTS(strtoq    HAVE_STRTOQ)
CHECK_FUNCTION_EXISTS(_strtoi64 HAVE__STRTOI64)

CHECK_TYPE_SIZE("long long"             LONG_LONG)
CHECK_TYPE_SIZE("unsigned long long"    UNSIGNED_LONG_LONG)

SET(BUILD_SHARED_LIBS OFF CACHE BOOL
    "Build shared libraries instead of static ones.")

OPTION(PCRE_BUILD_PCRECPP "Build the PCRE C++ library (pcrecpp)." ON)

SET(PCRE_EBCDIC OFF CACHE BOOL
    "Use EBCDIC coding instead of ASCII. (This is rarely used outside of mainframe systems)")

SET(PCRE_LINK_SIZE "2" CACHE STRING
    "Internal link size (2, 3 or 4 allowed). See LINK_SIZE in config.h.in for details.")

SET(PCRE_MATCH_LIMIT "10000000" CACHE STRING
    "Default limit on internal looping. See MATCH_LIMIT in config.h.in for details.")

SET(PCRE_MATCH_LIMIT_RECURSION "MATCH_LIMIT" CACHE STRING
    "Default limit on internal recursion. See MATCH_LIMIT_RECURSION in config.h.in for details.")

SET(PCRE_NEWLINE "LF" CACHE STRING
    "What to recognize as a newline (one of CR, LF, CRLF, ANY, ANYCRLF).")

SET(PCRE_NO_RECURSE OFF CACHE BOOL
    "If ON, then don't use stack recursion when matching. See NO_RECURSE in config.h.in for details.")

SET(PCRE_POSIX_MALLOC_THRESHOLD "10" CACHE STRING
    "Threshold for malloc() usage. See POSIX_MALLOC_THRESHOLD in config.h.in for details.")

SET(PCRE_SUPPORT_UNICODE_PROPERTIES OFF CACHE BOOL
    "Enable support for Unicode properties. (If set, UTF-8 support will be enabled as well)")

SET(PCRE_SUPPORT_UTF8 OFF CACHE BOOL
    "Enable support for the Unicode UTF-8 encoding.")

SET(PCRE_SUPPORT_BSR_ANYCRLF OFF CACHE BOOL
    "ON=Backslash-R matches only LF CR and CRLF, OFF=Backslash-R matches all Unicode Linebreaks")

OPTION(PCRE_SHOW_REPORT    "Show the final configuration report" ON)
OPTION(PCRE_BUILD_PCREGREP "Build pcregrep" ON)
OPTION(PCRE_BUILD_TESTS    "Build the tests" ON)

IF (PCRE_BUILD_TESTS)
  IF (NOT PCRE_BUILD_PCREGREP)
    MESSAGE(STATUS "** Building tests requires pcregrep: PCRE_BUILD_PCREGREP forced ON")
    SET(PCRE_BUILD_PCREGREP ON)
  ENDIF(NOT PCRE_BUILD_PCREGREP)
ENDIF(PCRE_BUILD_TESTS)

IF (MINGW)
  OPTION(NON_STANDARD_LIB_PREFIX
         "ON=Shared libraries built in mingw will be named pcre.dll, etc., instead of libpcre.dll, etc."
         OFF)

  OPTION(NON_STANDARD_LIB_SUFFIX
         "ON=Shared libraries built in mingw will be named libpcre-0.dll, etc., instead of libpcre.dll, etc."
         OFF)
ENDIF(MINGW)

# bzip2 lib
IF(BZIP2_FOUND)
  OPTION (PCRE_SUPPORT_LIBBZ2 "Enable support for linking pcregrep with libbz2." ON)
ENDIF(BZIP2_FOUND)
IF(PCRE_SUPPORT_LIBBZ2)
  INCLUDE_DIRECTORIES(${BZIP2_INCLUDE_DIR})
ENDIF(PCRE_SUPPORT_LIBBZ2)

# zlib
IF(ZLIB_FOUND)
  OPTION (PCRE_SUPPORT_LIBZ "Enable support for linking pcregrep with libz." ON)
ENDIF(ZLIB_FOUND)
IF(PCRE_SUPPORT_LIBZ)
  INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
ENDIF(PCRE_SUPPORT_LIBZ)

# readline lib
IF(READLINE_FOUND)
  OPTION (PCRE_SUPPORT_LIBREADLINE  "Enable support for linking pcretest with libreadline." ON)
ENDIF(READLINE_FOUND)
IF(PCRE_SUPPORT_LIBREADLINE)
  INCLUDE_DIRECTORIES(${READLINE_INCLUDE_DIR})
ENDIF(PCRE_SUPPORT_LIBREADLINE)

# Prepare build configuration

SET(pcre_have_type_traits 0)
SET(pcre_have_bits_type_traits 0)

IF(HAVE_TYPE_TRAITS_H)
        SET(pcre_have_type_traits 1)
ENDIF(HAVE_TYPE_TRAITS_H)

IF(HAVE_BITS_TYPE_TRAITS_H)
        SET(pcre_have_bits_type_traits 1)
ENDIF(HAVE_BITS_TYPE_TRAITS_H)

SET(pcre_have_long_long 0)
SET(pcre_have_ulong_long 0)

IF(HAVE_LONG_LONG)
        SET(pcre_have_long_long 1)
ENDIF(HAVE_LONG_LONG)

IF(HAVE_UNSIGNED_LONG_LONG)
        SET(pcre_have_ulong_long 1)
ENDIF(HAVE_UNSIGNED_LONG_LONG)

IF(NOT BUILD_SHARED_LIBS)
        SET(PCRE_STATIC 1)
ENDIF(NOT BUILD_SHARED_LIBS)

IF(PCRE_SUPPORT_BSR_ANYCRLF)
        SET(BSR_ANYCRLF 1)
ENDIF(PCRE_SUPPORT_BSR_ANYCRLF)

IF(PCRE_SUPPORT_UTF8 OR PCRE_SUPPORT_UNICODE_PROPERTIES)
        SET(SUPPORT_UTF8 1)
ENDIF(PCRE_SUPPORT_UTF8 OR PCRE_SUPPORT_UNICODE_PROPERTIES)

IF(PCRE_SUPPORT_UNICODE_PROPERTIES)
        SET(SUPPORT_UCP 1)
ENDIF(PCRE_SUPPORT_UNICODE_PROPERTIES)

# This next one used to contain
#       SET(PCRETEST_LIBS ${READLINE_LIBRARY})
# but I was advised to add the NCURSES test as well, along with
# some modifications to cmake/FindReadline.cmake which should
# make it possible to override the default if necessary. PH

IF(PCRE_SUPPORT_LIBREADLINE)
        SET(SUPPORT_LIBREADLINE 1)
        SET(PCRETEST_LIBS ${READLINE_LIBRARY} ${NCURSES_LIBRARY})
ENDIF(PCRE_SUPPORT_LIBREADLINE)

IF(PCRE_SUPPORT_LIBZ)
        SET(SUPPORT_LIBZ 1)
        SET(PCREGREP_LIBS ${PCREGREP_LIBS} ${ZLIB_LIBRARIES})
ENDIF(PCRE_SUPPORT_LIBZ)

IF(PCRE_SUPPORT_LIBBZ2)
        SET(SUPPORT_LIBBZ2 1)
        SET(PCREGREP_LIBS ${PCREGREP_LIBS} ${BZIP2_LIBRARIES})
ENDIF(PCRE_SUPPORT_LIBBZ2)

SET(NEWLINE "")

IF(PCRE_NEWLINE STREQUAL "LF")
        SET(NEWLINE "10")
ENDIF(PCRE_NEWLINE STREQUAL "LF")
IF(PCRE_NEWLINE STREQUAL "CR")
        SET(NEWLINE "13")
ENDIF(PCRE_NEWLINE STREQUAL "CR")
IF(PCRE_NEWLINE STREQUAL "CRLF")
        SET(NEWLINE "3338")
ENDIF(PCRE_NEWLINE STREQUAL "CRLF")
IF(PCRE_NEWLINE STREQUAL "ANY")
        SET(NEWLINE "-1")
ENDIF(PCRE_NEWLINE STREQUAL "ANY")
IF(PCRE_NEWLINE STREQUAL "ANYCRLF")
        SET(NEWLINE "-2")
ENDIF(PCRE_NEWLINE STREQUAL "ANYCRLF")

IF(NEWLINE STREQUAL "")
        MESSAGE(FATAL_ERROR "The PCRE_NEWLINE variable must be set to one of the following values: \"LF\", \"CR\", \"CRLF\", \"ANY\", \"ANYCRLF\".")
ENDIF(NEWLINE STREQUAL "")

IF(PCRE_EBCDIC)
        SET(EBCDIC 1)
ENDIF(PCRE_EBCDIC)

IF(PCRE_NO_RECURSE)
        SET(NO_RECURSE 1)
ENDIF(PCRE_NO_RECURSE)

# Output files
CONFIGURE_FILE(config-cmake.h.in
               ${PROJECT_BINARY_DIR}/pcre/config.h
               @ONLY)

CONFIGURE_FILE(pcre.h.generic
               ${PROJECT_BINARY_DIR}/pcre/pcre.h
               COPYONLY)



        CONFIGURE_FILE(pcre_stringpiece.h.in
                       ${PROJECT_BINARY_DIR}/pcre/pcre_stringpiece.h
                       @ONLY)

        CONFIGURE_FILE(pcrecpparg.h.in
                       ${PROJECT_BINARY_DIR}/pcre/pcrecpparg.h
                       @ONLY)
# Character table generation

CONFIGURE_FILE(pcre_chartables.c.dist
               ${PROJECT_BINARY_DIR}/pcre/pcre_chartables.c
               COPYONLY)

# Source code

SET(PCRE_HEADERS ${PROJECT_BINARY_DIR}/pcre/pcre.h)

SET(PCRE_SOURCES
  ${PROJECT_BINARY_DIR}/pcre/pcre_chartables.c
  pcre_compile.c
  pcre_config.c
  pcre_dfa_exec.c
  pcre_exec.c
  pcre_fullinfo.c
  pcre_get.c
  pcre_globals.c
  pcre_info.c
  pcre_newline.c
  pcre_maketables.c
  pcre_ord2utf8.c
  pcre_refcount.c
  pcre_study.c
  pcre_tables.c
  pcre_try_flipped.c
  pcre_ucd.c
  pcre_valid_utf8.c
  pcre_version.c
  pcre_xclass.c
)

SET(PCRECPP_SOURCES
        pcrecpp.cc
        pcre_scanner.cc
        pcre_stringpiece.cc
)
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

IF(MSVC)
        ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ENDIF(MSVC)

SET(CMAKE_INCLUDE_CURRENT_DIR 1)
IF(WIN32)
  SET(CMAKE_DEBUG_POSTFIX "d")
ENDIF(WIN32)

IF(NOT WIN32)
  ADD_DEFINITIONS(-fPIC)
ENDIF()

SET(targets)

INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/pcre)

ADD_LIBRARY(pcre ${PCRE_HEADERS} ${PCRE_SOURCES} ${PROJECT_BINARY_DIR}/pcre/config.h)
set_target_properties ( pcre PROPERTIES FOLDER core/mml/thirdparty )


ADD_LIBRARY(pcrecpp ${PCRECPP_HEADERS} ${PCRECPP_SOURCES})
set_target_properties ( pcrecpp PROPERTIES FOLDER core/mml/thirdparty )