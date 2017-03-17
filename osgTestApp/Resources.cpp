#include "Resources.h"
#include "Managers.h"

#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osgDB/readFile>
#include <osg/Geode>
#include <osg/PolygonMode>

//
// C3DObject
//

C3DObject::C3DObject(const osg::Vec3& pos)
{
	osg::PositionAttitudeTransform* pat = new osg::PositionAttitudeTransform;
	pat->setPosition(pos);

	_osgNode = pat;
	_osgNode->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

	osg::Matrix m;
	pat->computeLocalToWorldMatrix(m, nullptr);

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			_d3dTransformMatrix.m[i][j] = m(i, j);
}

C3DObject::C3DObject(osg::Geode* geode)
{
	_createdExternal = true;
	_osgNode = geode;

	osg::Matrix m = osg::Matrix::identity();

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			_d3dTransformMatrix.m[i][j] = m(i, j);
}

C3DObject::~C3DObject()
{
	if (!_createdExternal)
		_osgNode->asGroup()->removeChildren(0, _osgNode->asGroup()->getNumChildren());
}


void C3DObject::setFaceSets(const std::vector<C3DFaceSet*>& facesets)
{
	_faceSets = facesets;
	if (!_createdExternal)
	{

		for (C3DFaceSet* faceSet : _faceSets)
			_osgNode->asGroup()->addChild(faceSet->getOsgNode());
	}
}

// Функция возврящает количество мешей данного объекта
size_t C3DObject::GetMeshesCount() const
{
	return _meshes.size();
}

// Функция возвращает конкретный меш объекта по его идентификатору
C3DBaseMesh* C3DObject::GetMeshById(size_t id) const
{
	return _meshes[id];
}

// получить список фейссетов
size_t	 C3DObject::GetFaceSetsCount() const
{
	return _faceSets.size();
}

C3DBaseFaceSet*	 C3DObject::GetFaceSetById(size_t id) const
{
	return _faceSets[id];
}

// получить указатель на менеджер, управляющий данным ресурсом
// может быть NULL. В таком случае, ресурс не является выгружаемым
C3DBaseManager*	C3DObject::GetManager() const
{
	return &ObjectManager::Instance();
}


// Все 3D объекты должны будут возвращать Баунд-Бокс. Причем, если объект - точка, а не меш, то
// пусть вернет одинаковые значения в out_vBBMin и out_vBBMax.
void C3DObject::GetBoundBox(D3DXVECTOR3** ppBBMin, D3DXVECTOR3** ppBBMax)
{
	if (_createdExternal && getOsgNode()->asGeode())
	{
		osg::Vec3 vMin, vMax;
		vMin = getOsgNode()->asGeode()->getBoundingBox()._min;
		vMax = getOsgNode()->asGeode()->getBoundingBox()._max;

		_d3dBboxMin.x = vMin.x(); _d3dBboxMin.y = vMin.y(); _d3dBboxMin.z = vMin.z();
		_d3dBboxMax.x = vMax.x(); _d3dBboxMax.y = vMax.y(); _d3dBboxMax.z = vMax.z();
	}
	else
	{
		osg::BoundingSphere bound = _osgNode->getBound();
		osg::Vec3 minBound = bound.center() - osg::Vec3(bound.radius(), bound.radius(), bound.radius());
		osg::Vec3 maxBound = bound.center() + osg::Vec3(bound.radius(), bound.radius(), bound.radius());

		_d3dBboxMin.x = minBound.x(); _d3dBboxMin.y = minBound.y(); _d3dBboxMin.z = minBound.z();
		_d3dBboxMax.x = maxBound.x(); _d3dBboxMax.y = maxBound.y(); _d3dBboxMax.z = maxBound.z();
	}

	*ppBBMin = &_d3dBboxMin;
	*ppBBMax = &_d3dBboxMax;
}

// Получить матрицу трансформации
D3DXMATRIX* C3DObject::GetWorldTransform()
{
	return &_d3dTransformMatrix;
}

// Функция должна возвращать: включена-ли проверка размера объекта на экране
bool C3DObject::IsMinimalSizeCheckEnabled() const
{
	return false;
}

void C3DObject::setMeshes(const std::vector<C3DMesh*>& meshes)
{
	_meshes = meshes;

	if (!_createdExternal)
	{
		for (C3DMesh* mesh : meshes)
			_osgNode->asGroup()->addChild(mesh->getOsgNode());
	}
}

void C3DObject::SetPotentiallyVisible()
{
	if (_debugBoundBox)
	{
		// сделать баунд бокс видимым
		_debugBoundBox->setNodeMask(0xffffffff);
	}

	// этот обработчик нужно вызвать обязательно.
	C3DBaseObject::SetPotentiallyVisible();
}

//
// C3DMesh
//

C3DMesh::C3DMesh()
{
	_osgNode = new osg::Group;
}

C3DMesh::~C3DMesh()
{
	if (_osgNode->asGroup()->getNumChildren() > 0)
		_osgNode->asGroup()->removeChildren(0, _osgNode->asGroup()->getNumChildren());
}


//
//	C3DFaceSet
//

C3DFaceSet::C3DFaceSet(FaceSetType type)
{
	osg::Geode* geode = new osg::Geode;

	osg::Shape* shape = nullptr;

	switch (type)
	{
	case SPHERE:
		shape = new osg::Sphere(osg::Vec3(0,0,0), 0.5f);
		break;

	case CYLYNDER:
		shape = new osg::Cylinder(osg::Vec3(0,1,0), 0.3f, 1);
		break;

	case BOX:
		shape = new osg::Box(osg::Vec3(1, 0, 0), 0.2f, 0.4f, 0.6f);
		break;
	}

	if (shape)
		geode->addDrawable(new osg::ShapeDrawable(shape));

	_osgNode = geode;
}

C3DFaceSet::C3DFaceSet(osg::Geode* geode)
{
	_osgNode = geode;
}

void C3DFaceSet::setMaterialRef(C3DMaterial* materialRef)
{
	_materialRef = materialRef;

	if (materialRef->external())
	{
		_osgNode->getOrCreateStateSet()->setAttribute(materialRef->getOsgMaterial(), osg::StateAttribute::ON);

		int unit = 0;
		for (C3DTexture* texture : materialRef->textures())
		{
			_osgNode->getOrCreateStateSet()->setTextureAttributeAndModes(unit, texture->getOsgTexture(), osg::StateAttribute::ON);
			unit++;
		}
	}
}

C3DBaseMaterial* C3DFaceSet::GetMaterialRef()
{
	return _materialRef;
}

void C3DFaceSet::GetChildResources(std::vector<C3DBaseResource*>& out_vecChildResources) const
{
	out_vecChildResources.clear();
	out_vecChildResources.push_back(_materialRef);
}

C3DBaseManager*	C3DFaceSet::GetManager() const
{
	return nullptr;
}

//
// C3DMaterial
//

C3DMaterial::C3DMaterial(const osg::Vec4& diffuseColor)
{
	_osgMaterial = new osg::Material;
	_osgMaterial->setDiffuse(osg::Material::Face::FRONT_AND_BACK, diffuseColor);
	_storedDiffuse = diffuseColor;
	_storedSpecular = osg::Vec4(0, 0, 0, 0);
}

void C3DMaterial::makeBlack()
{
	_osgMaterial->setDiffuse(osg::Material::Face::FRONT_AND_BACK, osg::Vec4(0,0,0,0));
	_osgMaterial->setSpecular(osg::Material::Face::FRONT_AND_BACK, osg::Vec4(0, 0, 0, 0));
}

void C3DMaterial::restore()
{
	_osgMaterial->setDiffuse(osg::Material::Face::FRONT_AND_BACK, _storedDiffuse);
	_osgMaterial->setSpecular(osg::Material::Face::FRONT_AND_BACK, _storedSpecular);
}


C3DMaterial::C3DMaterial(osg::Material* material)
{
	_osgMaterial = material;
	_storedSpecular = _osgMaterial->getSpecular(osg::Material::Face::FRONT_AND_BACK);
	_storedDiffuse = _osgMaterial->getDiffuse(osg::Material::Face::FRONT_AND_BACK);
	makeBlack();

	_external = true;
}

C3DBaseManager*	C3DMaterial::GetManager() const
{ 
	return &MaterialManager::Instance(); 
};


void C3DMaterial::setTechniques(const std::vector<C3DTechnique*>& techniques)
{
	_techniques = techniques;
}

void C3DMaterial::setTextures(const std::vector<C3DTexture*>& textures)
{
	_textures = textures;
}

size_t	C3DMaterial::GetTechniquesCount() const
{
	return _techniques.size();
}

C3DBaseTechnique*	C3DMaterial::GetTechniqueById(size_t id) const
{
	return _techniques[id];
}

size_t C3DMaterial::GetTexturesCount() const
{
	return _textures.size();
}

C3DBaseTexture* C3DMaterial::GetTextureById(size_t id) const
{
	return _textures[id];
}



//
// C3DTechnique
//

C3DTechnique*	C3DTechnique::defaultTechnique()
{
	static C3DTechnique technique;
	return &technique;
}


//
// C3DTexture
// 

C3DTexture::C3DTexture(const std::string& fileName) : _fileName(fileName)
{
	_texture = new osg::Texture2D();

	_texture->setDataVariance(osg::Object::STATIC);
	_texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	_texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	_texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	_texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
}

// получить указатель на менеджер, управляющий данным ресурсом
// может быть NULL. В таком случае, ресурс не является выгружаемым
C3DBaseManager*	C3DTexture::GetManager() const
{
	return &TextureManager::Instance();
}

// выгрузить текстуру. Эта функция вызывается из менеджера текстур.
void C3DTexture::unloadTexture()
{
	std::cout << "unloading texture: " << _fileName << std::endl;

	_texture->setImage(nullptr);
}

// загрузить текстуру. Эта функция вызывается из менеджера текстур.
void C3DTexture::loadTexture()
{
	std::cout << "loading texture: " << _fileName << std::endl;

	osg::Image* image = osgDB::readImageFile(_fileName);
	_texture->setImage(image);
}
