set(TARGET_NAME zlib)

ENABLE_LANGUAGE(ASM_MASM)

set(ZLIB_PUBLIC_HDRS
    zconf.h
    zlib.h
)
set(ZLIB_PRIVATE_HDRS
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
set(ZLIB_SRCS
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

set(ZLIB_ASMS
    contrib/masmx86/inffas32.asm
    contrib/masmx86/match686.asm
)

add_library(${TARGET_NAME} STATIC ${ZLIB_SRCS} ${ZLIB_ASMS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS}) 

target_compile_definitions(${TARGET_NAME} PRIVATE ASMV ASMINF)

target_include_directories(${TARGET_NAME} INTERFACE .)
