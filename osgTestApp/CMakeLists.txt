find_package(OpenSceneGraph REQUIRED osgDB osgUtil osgText osgViewer osgGA)

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

add_executable(osgTestApp
	Managers.h
	Managers.cpp
	Resources.h
	Resources.cpp
	main.cpp
)

target_link_libraries(osgTestApp
	VisibilityManager
	ResourceManager
	${OPENSCENEGRAPH_LIBRARIES}
)

install(TARGETS osgTestApp
        RUNTIME DESTINATION bin)
