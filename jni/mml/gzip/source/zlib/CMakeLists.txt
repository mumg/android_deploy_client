set (zlib_source
    adler32.c
    compress.c
    crc32.c
    deflate.c
    gzclose.c
    gzlib.c
    gzread.c
    gzwrite.c
    inflate.c
    infback.c
    inftrees.c
    inffast.c
    trees.c
    uncompr.c
    zutil.c
)
SOURCE_GROUP ("Source Files" FILES ${zlib_source})

set (zlib_include
    zlib.h
    zconf.h
    crc32.h
    deflate.h
    gzguts.h
    inffast.h
    inffixed.h
    inflate.h
    inftrees.h
    trees.h
    zutil.h
)
SOURCE_GROUP ("Header Files" FILES ${zlib_include})


if(NOT MSVC)
add_definitions(-fPIC)
endif()

add_library(zlib
            ${zlib_source}
            ${zlib_include}
			)
set_target_properties ( zlib PROPERTIES FOLDER core/mml/thirdparty )