#include "Managers.h"
#include "Resources.h"

#include <osg/Group>
#include <osg/PolygonMode>
#include <osg/Geode>
#include <random>
#include <iostream>

namespace {
	std::default_random_engine generator;
};

ObjectManager&	ObjectManager::Instance()
{
	static ObjectManager instance;
	return instance;
}

ObjectManager::ObjectManager()
{
	_ptrRootNode = new osg::Group;
}

ObjectManager::~ObjectManager()
{
	for (C3DObject* object : _objects)
		delete object;

	_ptrRootNode->asGroup()->removeChildren(0, _ptrRootNode->asGroup()->getNumChildren());
}

void ObjectManager::GetObjectList(const D3DXVECTOR3& bboxMin, const D3DXVECTOR3& bboxMax, std::vector<C3DBaseObject*>& out_vecObjects)
{
	out_vecObjects.clear();
	for (C3DObject* object : _objects)
		out_vecObjects.push_back(object);
}

osg::Node* ObjectManager::getOsgRoot()
{
	return _ptrRootNode;
}

C3DObject* ObjectManager::generateTestObject(const osg::Vec3& pos)
{
	C3DObject* newObject = new C3DObject(pos);

	std::vector<C3DMesh*> vecMeshes;
	vecMeshes.push_back(MeshManager::Instance().getRandomMesh());

	newObject->setMeshes(vecMeshes);

	_ptrRootNode->asGroup()->addChild(newObject->getOsgNode());


	// create debug bounding box
	//@{
	osg::Box* debugBox = new osg::Box;

	osg::BoundingSphere bound = newObject->getOsgNode()->getBound();

	debugBox->setCenter(bound.center());
	debugBox->setHalfLengths(osg::Vec3(bound.radius(), bound.radius(), bound.radius()));

	osg::Geode* debugGeode = new osg::Geode();
	osg::ShapeDrawable* shape = new osg::ShapeDrawable(debugBox);
	shape->setColor(osg::Vec4(1, 0, 0, 1));
	debugGeode->addDrawable(shape);

	osg::PolygonMode * polygonMode = new osg::PolygonMode;
	polygonMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);

	debugGeode->getOrCreateStateSet()->setAttributeAndModes(polygonMode,
		osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

	_ptrRootNode->asGroup()->addChild(debugGeode);

	newObject->setupDebugBoundBox(debugGeode);
	//@}
	
	_objects.push_back(newObject);

	return newObject;
}

void ObjectManager::onBeginFrame()
{
	for (auto object : _objects)
	{
		object->debugBoundBoxNode()->setNodeMask(0);
	}
}

//
// MeshManager
//

MeshManager::MeshManager()
{
	generateMeshes();
}

MeshManager::~MeshManager()
{
	for (auto meshPtr : _meshes)
		delete meshPtr;
}

MeshManager& MeshManager::Instance()
{
	static MeshManager meshManager;
	return meshManager;
}

C3DMesh* MeshManager::getRandomMesh()
{
	if (_meshes.empty())
		return nullptr;

	std::uniform_int_distribution<size_t> distribution(0, _meshes.size() - 1);
	size_t diceRoll = distribution(generator);

	return _meshes[diceRoll];
}

void MeshManager::generateMeshes()
{
	const size_t NumMeshes = 50;

	for (int i = 0; i < NumMeshes; i++)
	{
		C3DMesh* mesh = new C3DMesh();

		std::vector<C3DFaceSet*> vecFaceSets;
		vecFaceSets.push_back(FaceSetManager::Instance().getRandomFaceSet());
		vecFaceSets.push_back(FaceSetManager::Instance().getRandomFaceSet());

		mesh->setFaceSets(vecFaceSets);

		_meshes.push_back(mesh);
	}
}

//
// FaceSetManager
//

FaceSetManager::FaceSetManager()
{
	generateFaceSets();
}

FaceSetManager::~FaceSetManager()
{
	for (auto fsPtr : _facesets)
		delete fsPtr;
}

FaceSetManager& FaceSetManager::Instance()
{
	static FaceSetManager faceSetManager;
	return faceSetManager;
}

C3DFaceSet*	FaceSetManager::getRandomFaceSet()
{
	if (_facesets.empty())
		return nullptr;

	std::uniform_int_distribution<size_t> distribution(0, _facesets.size() - 1);
	size_t diceRoll = distribution(generator);

	return _facesets[diceRoll];
}

void FaceSetManager::generateFaceSets()
{
	std::uniform_int_distribution<size_t> distribution(0, 2);

	C3DFaceSet::FaceSetType types[3] = { C3DFaceSet::CYLYNDER, C3DFaceSet::SPHERE, C3DFaceSet::BOX };
	const size_t NumFacesets = 30;

	for (int i = 0; i < NumFacesets; i++)
	{
		C3DFaceSet::FaceSetType type = types[distribution(generator)];
		C3DFaceSet* faceSet = new C3DFaceSet(type);

		faceSet->setMaterialRef(MaterialManager::Instance().getRandomMaterial());

		_facesets.push_back(faceSet);
	}
}

//
// MaterialManager
//

MaterialManager::MaterialManager()
{
	generateMaterialSet();
}

MaterialManager::~MaterialManager()
{
	for (auto matPtr : _materials)
		delete matPtr;
}

MaterialManager& MaterialManager::Instance()
{
	static MaterialManager materialManager;
	return materialManager;
}

// ������������� ����� ����������
void MaterialManager::generateMaterialSet()
{
	std::uniform_real_distribution<float> distribution(0.5, 2);

	const size_t NUM_MATERIALS = 10;
	for (int i = 0; i < NUM_MATERIALS; i++)
	{
		float r = distribution(generator);
		float g = distribution(generator);
		float b = distribution(generator);
		float a = distribution(generator);

		C3DMaterial* material = new C3DMaterial(osg::Vec4(r, g, b, a));

		std::vector<C3DTexture*> textures;
		textures.push_back(TextureManager::Instance().getRandomTexture());

		std::vector<C3DTechnique*> techniques;
		techniques.push_back(C3DTechnique::defaultTechnique());

		material->setTechniques(techniques);
		material->setTextures(textures);

		_materials.push_back(material);
	}
}

C3DMaterial* MaterialManager::getRandomMaterial()
{
	if (_materials.empty())
		return nullptr;

	std::uniform_int_distribution<size_t> distribution(0, _materials.size() - 1);
	size_t diceRoll = distribution(generator);

	return _materials[diceRoll];
}

//
// TextureManager
//

TextureManager::TextureManager()
{
	generateTextureSet();
}

TextureManager::~TextureManager()
{
	for (auto texturePtr : _textures)
		delete texturePtr;
}

TextureManager&	TextureManager::Instance()
{
	static TextureManager textureManager;
	return textureManager;
}

//@{ C3DBaseManager

// ��������� �������� �������
void TextureManager::RequestLoadResource(C3DBaseResource* resource)
{
	// ���� ��� ������� �� ��������� � ������, ��� ������� - ������ �� ������
	if (!resource || resource->GetType() != C3DRESOURCE_TEXTURE)
		return;

	static_cast<C3DTexture*>(resource)->loadTexture();
}

// ��������� �������� �������
void TextureManager::RequestUnloadResource(C3DBaseResource* resource)
{
	// ���� ��� ������� �� ��������� � ������, ��� ������� - ������ �� ������
	if (!resource || resource->GetType() != C3DRESOURCE_TEXTURE)
		return;

	static_cast<C3DTexture*>(resource)->unloadTexture();
}

//@} C3DBaseManager

// �������� ��������� �������� �� ���������������� ������
C3DTexture*	TextureManager::getRandomTexture()
{
	if (_textures.empty())
		return nullptr;

	std::uniform_int_distribution<size_t> distribution(0, _textures.size() - 1);
	size_t diceRoll = distribution(generator);

	std::cout << "Texture dice: " << diceRoll << std::endl;

	return _textures[diceRoll];
}

void TextureManager::generateTextureSet()
{
	_textures.push_back(new C3DTexture("../data/textures/texture1.jpg"));
	_textures.push_back(new C3DTexture("../data/textures/texture2.jpg"));
	_textures.push_back(new C3DTexture("../data/textures/texture3.jpg"));
	_textures.push_back(new C3DTexture("../data/textures/texture4.jpg"));
	_textures.push_back(new C3DTexture("../data/textures/texture5.jpg"));
}
