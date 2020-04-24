cmake_minimum_required( VERSION 3.10.0 )

set( PS4 YES )
set( CMAKE_SYSTEM_NAME Generic )
set( CMAKE_SYSTEM_VERSION 1 )
set( CMAKE_SYSTEM_PROCESSOR "x86_64" )
set( CMAKE_C_COMPILER "${PS4_SDK}/host_tools/bin/orbis-clang.exe" CACHE PATH "C compiler" )
set( CMAKE_CXX_COMPILER "${PS4_SDK}/host_tools/bin/orbis-clang++.exe" CACHE PATH "C++ compiler" )
set( CMAKE_ASM_COMPILER "${PS4_SDK}/host_tools/bin/orbis-as.exe" CACHE PATH "assembler" )
set( CMAKE_STRIP "${PS4_SDK}/host_tools/bin/orbis-strip.exe" CACHE PATH "strip" )
set( CMAKE_AR "${PS4_SDK}/host_tools/bin/orbis-ar.exe" CACHE PATH "archive" )
set( CMAKE_LINKER "${PS4_SDK}/host_tools/bin/orbis-ld.exe" CACHE PATH "linker" )
set( CMAKE_NM "${PS4_SDK}/host_tools/bin/orbis-nm.exe" CACHE PATH "nm" )
set( CMAKE_OBJCOPY "${PS4_SDK}/host_tools/bin/orbis-objcopy.exe" CACHE PATH "objcopy" )
set( CMAKE_OBJDUMP "${PS4_SDK}/host_tools/bin/orbis-objdump.exe" CACHE PATH "objdump" )
set( CMAKE_RANLIB "${PS4_SDK}/host_tools/bin/orbis-ranlib.exe" CACHE PATH "ranlib" )
set( CMAKE_C_COMPILER_ID Clang )
set( CMAKE_C_PLATFORM_ID Linux )
set( CMAKE_C_SIZEOF_DATA_PTR 8 )
set( CMAKE_C_HAS_ISYSROOT 1 )
set( CMAKE_C_COMPILER_ABI ELF )
set( CMAKE_CXX_COMPILER_ID Clang)
set( CMAKE_CXX_PLATFORM_ID Linux )
set( CMAKE_CXX_SIZEOF_DATA_PTR 8 )
set( CMAKE_CXX_HAS_ISYSROOT 1 )
set( CMAKE_CXX_COMPILER_ABI ELF )
set( CMAKE_CXX_SOURCE_FILE_EXTENSIONS cc cp cxx cpp CPP c++ C )
set( CMAKE_CXX_CREATE_SHARED_LIBRARY "<CMAKE_CXX_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>" )
set( CMAKE_CXX_CREATE_SHARED_MODULE "<CMAKE_CXX_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>" )
set( CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>" )
set( CMAKE_CXX_FLAGS "" CACHE STRING "c++ flags" )
set( CMAKE_C_FLAGS "" CACHE STRING "c flags" )
set( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "c++ Release flags" )
set( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "c Release flags" )
set( CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG -D_DEBUG" CACHE STRING "c++ Debug flags" )
set( CMAKE_C_FLAGS_DEBUG "-O0 -g -DDEBUG -D_DEBUG" CACHE STRING "c Debug flags" )
set( CMAKE_SHARED_LINKER_FLAGS "" CACHE STRING "shared linker flags" )
set( CMAKE_MODULE_LINKER_FLAGS "" CACHE STRING "module linker flags" )
set( CMAKE_EXE_LINKER_FLAGS "-Wall" CACHE STRING "executable linker flags" )
set( CMAKE_POSITION_INDEPENDENT_CODE TRUE )
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie" )
# set( CMAKE_POSITION_INDEPENDENT_CODE FALSE )
# set( CMAKE_CXX_FLAGS "-fpic ${CMAKE_CXX_FLAGS}" )
# set( CMAKE_C_FLAGS "-fpic ${CMAKE_C_FLAGS}" )
set( CMAKE_FIND_ROOT_PATH "${PS4_SDK}/target" )
set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
set( CMAKE_CROSSCOMPILING TRUE )