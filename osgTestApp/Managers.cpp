#include "Managers.h"
#include "Resources.h"

#include <osg/Group>
#include <osg/PolygonMode>
#include <osg/MatrixTransform>
#include <osg/Billboard>
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

//
//void ObjectManager::GetObjectList(const D3DXVECTOR3& bboxMin, const D3DXVECTOR3& bboxMax, std::vector<C3DBaseObject*>& out_vecObjects)
//{
//	out_vecObjects.clear();
//	for (C3DObject* object : _objects)
//		out_vecObjects.push_back(object);
//}

size_t ObjectManager::GetObjectsCount() const
{
	return _objects.size();
}

C3DBaseObject*	ObjectManager::GetObjectByIndex(size_t id) const
{
	return _objects[id];
}


osg::Node* ObjectManager::getOsgRoot()
{
	return _ptrRootNode;
}

C3DObject* ObjectManager::generateTestObject(const osg::Vec3& pos)
{
	C3DObject* newObject = new C3DObject(pos);

	std::vector<C3DFaceSet*> vecFaceSets;
	vecFaceSets.push_back(FaceSetManager::Instance().getRandomFaceSet());
	vecFaceSets.push_back(FaceSetManager::Instance().getRandomFaceSet());

	newObject->setFaceSets(vecFaceSets);

	std::vector<C3DMesh*> vecMeshes;
	vecMeshes.push_back(MeshManager::Instance().getRandomMesh());

	newObject->setMeshes(vecMeshes);

	_ptrRootNode->asGroup()->addChild(newObject->getOsgNode());


	// create debug bounding box
	//@{
	createDebugBoundBox(newObject);
	//@}
	
	_objects.push_back(newObject);

	return newObject;
}

void ObjectManager::createDebugBoundBox(C3DObject* newObject)
{
	osg::Box* debugBox = new osg::Box;

	//osg::BoundingSphere bound = newObject->getOsgNode()->getBound();

	if (newObject->getOsgNode()->asGeode())
	{
		osg::Vec3 vMin, vMax;
		vMin = newObject->getOsgNode()->asGeode()->getBoundingBox()._min;
		vMax = newObject->getOsgNode()->asGeode()->getBoundingBox()._max;

		debugBox->setCenter(newObject->getOsgNode()->asGeode()->getBoundingBox().center());
		debugBox->setHalfLengths((vMax - vMin)*0.5);
	}
	else
	{
		osg::BoundingSphere bound = newObject->getOsgNode()->computeBound();
		debugBox->setCenter(bound.center());
		debugBox->setHalfLengths(osg::Vec3(bound.radius(), bound.radius(), bound.radius()));
	}

	osg::Geode* debugGeode = new osg::Geode();
	osg::ShapeDrawable* shape = new osg::ShapeDrawable(debugBox);
	shape->setColor(osg::Vec4(1, 0, 0, 1));
	debugGeode->addDrawable(shape);

	osg::PolygonMode * polygonMode = new osg::PolygonMode;
	polygonMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);

	debugGeode->getOrCreateStateSet()->setAttributeAndModes(polygonMode,
		osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

	debugGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

	_ptrRootNode->asGroup()->addChild(debugGeode);

	newObject->setupDebugBoundBox(debugGeode);
}


void ObjectManager::addObjectsRecursiveFromOsgNode(osg::Node* node)
{
	static std::map<osg::Material*, C3DMaterial*> mapMaterials;

	if (osg::Geode* geode = node->asGeode())
	{
		if (geode->getNumDrawables() > 0)
		{
			C3DObject* newObject = new C3DObject(geode);

			std::vector<C3DFaceSet*> facesets;

			for (unsigned int i = 0; i < geode->getNumDrawables(); i++)
			{
				osg::Drawable* drawable = geode->getDrawable(i);
				C3DFaceSet* faceset = new C3DFaceSet(geode);

				osg::ref_ptr<osg::Material> mat = (osg::Material*)drawable->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL);
				if (mat.valid())
				{
					C3DMaterial*& myMat = mapMaterials[mat];

					if (!myMat)
						myMat = new C3DMaterial(mat);

					faceset->setMaterialRef(myMat);
				}

				facesets.push_back(faceset);
			}
			

			C3DMesh* mesh = new C3DMesh();

			std::vector<C3DMesh*> vecMeshes;
			vecMeshes.push_back(mesh);

			newObject->setMeshes(vecMeshes);

			newObject->setFaceSets(facesets);

			createDebugBoundBox(newObject);

			_objects.push_back(newObject);
		}
	}


	if (osg::Group* group = node->asGroup())
	{
		for (unsigned int i = 0; i < group->getNumChildren(); i++)
		{
			addObjectsRecursiveFromOsgNode(group->getChild(i));
		}
	}
}

void ObjectManager::generateObjectsFromOsgNode(osg::Node* node)
{
	_ptrRootNode->asGroup()->addChild(node);

	addObjectsRecursiveFromOsgNode(node);
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

// —генерировать набор материалов
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

// «апросить загрузку ресурса
void MaterialManager::RequestLoadResource(C3DBaseResource* res)
{
	C3DMaterial* mat = static_cast<C3DMaterial*>(res);
	mat->restore();
}

// запросить выгрузку ресурса
void MaterialManager::RequestUnloadResource(C3DBaseResource* res)
{
	C3DMaterial* mat = static_cast<C3DMaterial*>(res);
	mat->makeBlack();
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

// «апросить загрузку ресурса
void TextureManager::RequestLoadResource(C3DBaseResource* resource)
{
	// ≈сли тип ресурса не совпадает с нужным, или нулевой - ничего не делаем
	if (!resource || resource->GetType() != C3DRESOURCE_TEXTURE)
		return;

	static_cast<C3DTexture*>(resource)->loadTexture();
}

// запросить выгрузку ресурса
void TextureManager::RequestUnloadResource(C3DBaseResource* resource)
{
	// ≈сли тип ресурса не совпадает с нужным, или нулевой - ничего не делаем
	if (!resource || resource->GetType() != C3DRESOURCE_TEXTURE)
		return;

	static_cast<C3DTexture*>(resource)->unloadTexture();
}

//@} C3DBaseManager

// получить случайную текстуру из сгенерированного набора
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
