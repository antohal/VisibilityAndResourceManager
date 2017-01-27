#pragma once

#include "3DEngineInterface.h"
#include "Resources.h"

#include <osg/Node>

//
// ObjectManager
//
// ������������� ������ ��������� ��������
//

class ObjectManager : public C3DBaseObjectManager
{
public:

	ObjectManager();
	~ObjectManager();

	static ObjectManager&	Instance();

	//@{ C3DBaseObjectManager
	
	virtual size_t GetObjectsCount() const ;
	virtual C3DBaseObject*	GetObjectByIndex(size_t id) const ;

	//@}

	//@{ C3DBaseManager

	// ��������� �������� �������
	virtual void RequestLoadResource(C3DBaseResource*) {};

	// ��������� �������� �������
	virtual void RequestUnloadResource(C3DBaseResource*) {};

	//@} C3DBaseManager

	// �������� ������ � ����� ������ osg
	osg::Node*	getOsgRoot();

	// ������������� �������� ������ �� ������ ��� ��������� �����, ���������, ����������, ������� � �.�.
	C3DObject*	generateTestObject(const osg::Vec3& pos);

	// ������� ������� �� ������������ ������
	void generateObjectsFromOsgNode(osg::Node*);

	// � ���� ������� �������������� ������ ����� ��� ���������� �����
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
// �������� �����. ����� ��������� �������� ����� �����, � �������� ������ ���������.
// �� ����������� �� C3DBaseManager, ��������� � ������ ������� ���� ��� �������������.
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
// �������� ���������. ����� ��������� �������� ����� ���������, � �������� ������ ���������.
// �� ����������� �� C3DBaseManager, ��������� � ������ ������� ���� ��� �������������.
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
// �������� ����������. ����� ��������� �������� ����� ����������, � �������� ������ ���������.
// �� ����������� �� C3DBaseManager, ��������� � ������ ������� ���� ��� �������������.
//

class MaterialManager : public C3DBaseManager
{
public:

	MaterialManager();
	~MaterialManager();

	static MaterialManager&		Instance();

	C3DMaterial*	getRandomMaterial();

	// ��������� �������� �������
	virtual void RequestLoadResource(C3DBaseResource*);

	// ��������� �������� �������
	virtual void RequestUnloadResource(C3DBaseResource*);

private:

	void generateMaterialSet();

	std::vector<C3DMaterial*>	_materials;
};



//
// TextureManager
//
// �������� �������. ����� ��������� �������� ����� �������, � �������� ������ ���������.
//

class TextureManager : public C3DBaseManager
{
public:

	TextureManager();
	~TextureManager();

	static TextureManager&	Instance();

	//@{ C3DBaseManager

	// ��������� �������� �������
	virtual void RequestLoadResource(C3DBaseResource*);

	// ��������� �������� �������
	virtual void RequestUnloadResource(C3DBaseResource*);

	//@} C3DBaseManager

	// �������� ��������� �������� �� ���������������� ������
	C3DTexture*	getRandomTexture();

private:

	void generateTextureSet();

	std::vector<C3DTexture*>	_textures;
};