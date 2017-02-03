include_directories(${PROJECT_SOURCE_DIR}/include/)

set(ContainersSrc
	Containers/BitField.cpp
	Containers/BitField.h
	Containers/Store.h)
	
set(GeometrySrc
	Geometry/BoundBox.h
	Geometry/IndependenceGeometryFunctions.h
	Geometry/InFrustum.h
	Geometry/Matrix.cpp
	Geometry/Matrix.h
	Geometry/Matrix.inl
	Geometry/Plane.h
	Geometry/Vector3D.h
	Geometry/Vector3D.cpp)
	
set(STLStringSrc
	STLString/StlUtil.cpp
	STLString/StlUtil.h)

set(InterfaceHeaders
	${PROJECT_SOURCE_DIR}/include/3DEngineInterface.h
	${PROJECT_SOURCE_DIR}/include/ResourceManager.h

	${PROJECT_SOURCE_DIR}/include/C3DBaseFaceSet.h
	${PROJECT_SOURCE_DIR}/include/C3DBaseManager.h
	${PROJECT_SOURCE_DIR}/include/C3DBaseMaterial.h
	${PROJECT_SOURCE_DIR}/include/C3DBaseMesh.h
	${PROJECT_SOURCE_DIR}/include/C3DBaseObject.h
	${PROJECT_SOURCE_DIR}/include/C3DBaseObjectManager.h
	${PROJECT_SOURCE_DIR}/include/C3DBaseResource.h
	${PROJECT_SOURCE_DIR}/include/C3DBaseTechnique.h
	${PROJECT_SOURCE_DIR}/include/C3DBaseTexture.h

	${PROJECT_SOURCE_DIR}/include/VisibilityManager.h)
	
source_group(Containers FILES ${ContainersSrc})
source_group(Geometry FILES ${GeometrySrc})
source_group(STLString FILES ${STLStringSrc})
source_group(Interface FILES ${InterfaceHeaders})

add_library(VisibilityManager SHARED
	${ContainersSrc}
	${GeometrySrc}
	${STLStringSrc}
	${InterfaceHeaders}
	Camera.h
	Camera.cpp
	Common.cpp
	Common.h
	CrashHandler.cpp
	Debug.cpp
	Debug.h
	FuncForOkTree.h
	GridIterator.h
	GridManager.cpp
	GridManager.h
	IGrid.h
	Log.h
	Log.cpp
	OkTree.cpp
	OkTree.h
	OkTreeGridIterator.h
	OkTreeGridIterator.cpp
	VisibilityManager.cpp)
	
target_link_libraries(VisibilityManager
#	${DirectX_D3DX10_LIBRARY}
	${TEXT_RENDERER_LIBRARY})

install(TARGETS VisibilityManager RUNTIME DESTINATION ./
	ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/lib)

add_library(ResourceManager SHARED
	${InterfaceHeaders}
	${GeometrySrc}
	Log.h
	Log.cpp
	ResourceManager.cpp)
	
target_link_libraries(ResourceManager 
	VisibilityManager
	${TEXT_RENDERER_LIBRARY})

install(TARGETS ResourceManager RUNTIME DESTINATION ./
	ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/lib)
