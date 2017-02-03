cmake_minimum_required(VERSION 2.8)

project(VisibilityAndResourceManager)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeModules")

find_package(DirectX)

#include_directories(${DirectX_D3DX10_INCLUDE_DIR})


find_path(TEXT_RENDERER_INCLUDE_DIR NAMES CDirect2DTextRenderer.h)
find_library(TEXT_RENDERER_LIBRARY NAMES D2DTextRender)

message(STATUS "Found text renderer include dir: " ${TEXT_RENDERER_INCLUDE_DIR})
message(STATUS "Found text renderer library: " ${TEXT_RENDERER_LIBRARY})

include_directories(${TEXT_RENDERER_INCLUDE_DIR})


find_path(D3DX_INCLUDE_PATH NAMES D3DX10math.h)
find_library(D3DX10_LIBRARY NAMES d3dx10)
find_library(D3DX9_LIBRARY NAMES d3dx9)

message(STATUS "Found d3dx10 include dir: " ${D3DX_INCLUDE_PATH})
message(STATUS "Found d3dx10 library: " ${D3DX10_LIBRARY})
message(STATUS "Found d3dx9 library: " ${D3DX9_LIBRARY})

include_directories(${D3DX_INCLUDE_PATH})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(src)

option(COMPILE_TEST_APP "Compile Test Library" ON)

if (COMPILE_TEST_APP)
	add_subdirectory(osgTestApp)
endif()
