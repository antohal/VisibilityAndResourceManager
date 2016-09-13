cmake_minimum_required(VERSION 2.8)

project(VisibilityAndResourceManager)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeModules")

find_package(DirectX)

include_directories(${DirectX_D3DX10_INCLUDE_DIR})


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(src)

option(COMPILE_TEST_APP "Compile Test Library" ON)

if (COMPILE_TEST_APP)
	add_subdirectory(osgTestApp)
endif()
