SET(ISA_LINKER_FLAGS "${ISA_LINKER_FLAGS}")

#set(CMAKE_CXX_LINK_EXECUTABLE "/usr/local/cross/bin/${ARCH}-elf-ld -N --eh-frame-hdr -T ${PLATFORM_LAYOUT} <OBJECTS> -o <TARGET> <LINK_LIBRARIES>-L/usr/tooLargeForHome/cross_src/build-gcc/i686-elf/libgcc/ -lgcc")

SET(ISA_LINKER_FLAGS "${ISA_LINKER_FLAGS} CMakeFiles/LudOS.dir/kern/i686/pc/multiboot.asm.obj")

target_compile_options(${PROJECT_NAME} PUBLIC $<$<BOOL:$<COMPILE_LANGUAGE:ASM_NASM>>:-I${CMAKE_CURRENT_SOURCE_DIR}/kern/i686/>) #necessary for nasm

configure_file("${PROJECT_SOURCE_DIR}/kern/i686/pc/layout.ld.in" "${PROJECT_SOURCE_DIR}/kern/i686/pc/layout.ld")
