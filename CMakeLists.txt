cmake_minimum_required(VERSION 3.10)
project(frld VERSION 0.0.1 DESCRIPTION "frld 001")

#option(TINYGLTF_HEADER_ONLY "On: header-only mode. Off: create tinygltf library(No TINYGLTF_IMPLEMENTATION required in your project)" OFF)

include(GNUInstallDirs)

if(WIN32)
	add_compile_definitions(_WIN32)
endif()

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${ZLIB_INCLUDE_DIRS})

include_directories(cmem)
include_directories(umem)

#find . "*.cpp"
set(SOURCES
	src/formats/dcx/dcx.cpp
	src/formats/binder/bnd/bnd3.cpp
	src/formats/binder/bnd/bnd4.cpp
	src/formats/binder/binder.cpp
	src/formats/flver/flver0/flver0.cpp
	src/formats/flver/flver.cpp
	src/formats/flver/flver2/flver2.cpp
	src/formats/flver/flver2/flver2_helpers.cpp
	#src/formats/flver/flver2/flver2_gltf_converter.cpp
	#src/formats/flver/flver2/tiny_gltf.cpp
	src/formats/mtd/mtd.cpp
	src/formats/paramdef/paramdef.cpp
	src/open.cpp
	src/util/inflate/inflate.cpp
	src/util/validate/validate.cpp
	src/util/fileio/fileio.cpp
	src/util/endian/endian.cpp
	src/util/stringio/stringio.cpp
	src/util/types/types.cpp
	src/util/file/file.cpp
)

add_library(frld STATIC ${SOURCES} "fromloader.h")

target_link_libraries(frld cmem umem ZLIB::ZLIB)

target_include_directories(frld PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})