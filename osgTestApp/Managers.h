#pragma once

#include "3DEngineInterface.h"
#include "Resources.h"

#include <osg/Node>

//
// ObjectManager
//
// Специализация класса менеджера объектов
//

class ObjectManager : public C3DBaseObjectManager
{
public:

	ObjectManager();
	~ObjectManager();

	static ObjectManager&	Instance();

	//@{ C3DBaseObjectManager
	
	virtual void GetObjectList(const D3DXVECTOR3& bboxMin, const D3DXVECTOR3& bboxMax, std::vector<C3DBaseObject*>& out_vecObjects);
	
	//@}

	//@{ C3DBaseManager

	// Запросить загрузку ресурса
	virtual void RequestLoadResource(C3DBaseResource*) {};

	// запросить выгрузку ресурса
	virtual void RequestUnloadResource(C3DBaseResource*) {};

	//@} C3DBaseManager

	// получить доступ к корню дерева osg
	osg::Node*	getOsgRoot();

	// сгенерировать тестовый объект из набора уже созданных мешей, фейссетов, материалов, текстур и т.п.
	C3DObject*	generateTestObject(const osg::Vec3& pos);

	// считать ресурсы из загруженного дерева
	void generateObjectsFromOsgNode(osg::Node*);

	// в этой функции обрабатывается начало кадра для отладочных целей
	void onBeginFrame();

private:

	void addObjectsRecursiveFromOsgNode(osg::Node* node);
	void createDebugBoundBox(C3DObject* newObject);

	osg::ref_ptr<osg::Node>		_ptrRootNode;
	std::vector<C3DObject*>		_objects;
};

//
// MeshManager
//
// Менеджер мешей. Умеет создавать тестовый набор мешей, и выдавать оттуда случайный.
// не наследуется от C3DBaseManager, поскольку в данном примере пока нет необходимости.
//

class MeshManager
{
public:

	MeshManager();
	~MeshManager();

	static MeshManager&		Instance();

	C3DMesh*	getRandomMesh();

private:

	void	generateMeshes();

	std::vector<C3DMesh*>		_meshes;
};


//
// FaceSetManager
//
// Менеджер фейссетов. Умеет создавать тестовый набор фейссетов, и выдавать оттуда случайный.
// не наследуется от C3DBaseManager, поскольку в данном примере пока нет необходимости.
//

class FaceSetManager
{
public:

	FaceSetManager();
	~FaceSetManager();

	static FaceSetManager&		Instance();

	C3DFaceSet*	getRandomFaceSet();

private:

	void generateFaceSets();

	std::vector<C3DFaceSet*>	_facesets;
};


//
// MaterialManager
//
// Менеджер материалов. Умеет создавать тестовый набор материалов, и выдавать оттуда случайный.
// не наследуется от C3DBaseManager, поскольку в данном примере пока нет необходимости.
//

class MaterialManager
{
public:

	MaterialManager();
	~MaterialManager();

	static MaterialManager&		Instance();

	C3DMaterial*	getRandomMaterial();

private:

	void generateMaterialSet();

	std::vector<C3DMaterial*>	_materials;
};



//
// TextureManager
//
// Менеджер текстур. Умеет создавать тестовый набор текстур, и выдавать оттуда случайный.
//

class TextureManager : public C3DBaseManager
{
public:

	TextureManager();
	~TextureManager();

	static TextureManager&	Instance();

	//@{ C3DBaseManager

	// Запросить загрузку ресурса
	virtual void RequestLoadResource(C3DBaseResource*);

	// запросить выгрузку ресурса
	virtual void RequestUnloadResource(C3DBaseResource*);

	//@} C3DBaseManager

	// получить случайную текстуру из сгенерированного набора
	C3DTexture*	getRandomTexture();

private:

	void generateTextureSet();

	std::vector<C3DTexture*>	_textures;
};