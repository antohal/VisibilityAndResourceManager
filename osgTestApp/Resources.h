#pragma once

#include <osg/Node>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/ShapeDrawable>

#include "D3DX10.h"

#include "3DEngineInterface.h"

//@{ Предопределения
class C3DFaceSet; 
class C3DMesh;
class C3DMaterial;
class C3DTechnique;
class C3DTexture;
//@}

//
// Класс 3Д объекта. Содержит в качестве дочерних ресурсов - список мешей.
//

class C3DObject : public C3DBaseObject
{
public:

	C3DObject(const osg::Vec3& pos);
	C3DObject(osg::Geode* geode);
	~C3DObject();

	osg::Node*	getOsgNode() const { return _osgNode; }

	//@{ C3DBaseResource

	// получить дочерние ресурсы
	virtual void GetChildResources(std::vector<C3DBaseResource*>& out_vecChildResources) const;

	// получить указатель на менеджер, управляющий данным ресурсом
	// может быть NULL. В таком случае, ресурс не является выгружаемым
	virtual C3DBaseManager*	GetManager() const;

	//@} C3DBaseResource


	//@{ C3DBaseObject

	// Все 3D объекты должны будут возвращать Баунд-Бокс. Причем, если объект - точка, а не меш, то
	// пусть вернет одинаковые значения в out_vBBMin и out_vBBMax.
	virtual void GetBoundBox(D3DXVECTOR3** ppBBMin, D3DXVECTOR3** ppBBMax);

	// Получить матрицу трансформации
	virtual D3DXMATRIX* GetWorldTransform();

	// Функция должна возвращать: включена-ли проверка размера объекта на экране
	virtual bool IsMinimalSizeCheckEnabled() const;

	// Функция возврящает набор мешей данного объекта
	virtual void	GetMeshes(std::vector<C3DBaseMesh*>& out_vecMeshes) const;

	// функция вызывается, когда объект становится видимым
	virtual void	SetPotentiallyVisible();

	//@} C3DBaseObject

	void setMeshes(const std::vector<C3DMesh*>&);

	void setupDebugBoundBox(osg::Node* node){ _debugBoundBox = node; };
	osg::Node* debugBoundBoxNode() const { return _debugBoundBox; }

private:

	std::vector<C3DMesh*>		_meshes;
	osg::ref_ptr<osg::Node>		_osgNode;

	osg::ref_ptr<osg::Node>		_debugBoundBox;

	D3DXVECTOR3					_d3dBboxMin;
	D3DXVECTOR3					_d3dBboxMax;
	D3DXMATRIX					_d3dTransformMatrix;

	bool						_createdExternal = false;
};

//
// Класс меша. Содержит в качестве дочерних ресурсов - список фейссетов.
//

class C3DMesh : public C3DBaseMesh
{
public:

	C3DMesh();
	~C3DMesh();

	osg::Node*	getOsgNode() const { return _osgNode; }

	void setFaceSets(const std::vector<C3DFaceSet*>&);

	//@{ C3DBaseResource
	
	// получить дочерние ресурсы
	virtual void GetChildResources(std::vector<C3DBaseResource*>& out_vecChildResources) const;

	// получить указатель на менеджер, управляющий данным ресурсом
	// может быть NULL. В таком случае, ресурс не является выгружаемым
	virtual C3DBaseManager*	GetManager() const { return nullptr; }

	//@} C3DBaseResource


	//@{ C3DBaseMesh

	// Получить список Фейс-Сетов, используемых в меше
	virtual void	GetFaceSets(std::vector<C3DBaseFaceSet*>& out_vecFaceSets) const;

	//@} C3DBaseMesh

private:
	
	osg::ref_ptr<osg::Node>		_osgNode;
	std::vector<C3DFaceSet*>	_faceSets;
};

//
// Класс фейссета. Содержит в качестве дочерних ресурсов - ссылку на материал.
//

class C3DFaceSet : public C3DBaseFaceSet
{
public:

	enum FaceSetType{ CYLYNDER, BOX, SPHERE };

	C3DFaceSet(FaceSetType);
	C3DFaceSet(osg::Geode*);

	osg::Node*	getOsgNode() const { return _osgNode; }

	void setMaterialRef(C3DMaterial* materialRef);

	//@{ C3DBaseFaceSet
	
	virtual C3DBaseMaterial* GetMaterialRef();

	//@} C3DBaseFaceSet

	//@{ C3DBaseResource

	// получить дочерние ресурсы. У текстур нет дочерних ресурсов.
	virtual void GetChildResources(std::vector<C3DBaseResource*>& out_vecChildResources) const;

	// получить указатель на менеджер, управляющий данным ресурсом
	// может быть NULL. В таком случае, ресурс не является выгружаемым
	virtual C3DBaseManager*	GetManager() const;

	//@} C3DBaseResource

private:

	osg::ref_ptr<osg::Node>		_osgNode;
	C3DMaterial*				_materialRef = nullptr;
};

class C3DMaterial : public C3DBaseMaterial
{
public:

	C3DMaterial(const osg::Vec4& diffuseColor);
	C3DMaterial(osg::Material*);

	//@{ C3DBaseResource

	// дочерних ресурсов у техники нет.
	virtual void GetChildResources(std::vector<C3DBaseResource*>& out_vecChildResources) const;

	virtual C3DBaseManager*	GetManager() const;

	//@} C3DBaseResource

	//@{ C3DBaseMaterial

	virtual void	AddVisibleFaceSet(C3DBaseFaceSet*) {}

	// Получить набор техник
	virtual void	GetTechniques(std::vector<C3DBaseTechnique*>& out_vecTechniques) const;

	// получить список текстур
	virtual void	GetTextures(std::vector<C3DBaseTexture*>& out_vecTextures) const;

	//@} C3DBaseMaterial

	void setTechniques(const std::vector<C3DTechnique*>&);
	void setTextures(const std::vector<C3DTexture*>&);
	osg::Material*	getOsgMaterial() const { return _osgMaterial; }

	const std::vector<C3DTexture*>& textures() const { return _textures; }

	void makeBlack();
	void restore();

	bool external() const {
		return _external;
	}

private:

	osg::ref_ptr<osg::Material>	_osgMaterial;
	std::vector<C3DTechnique*>	_techniques;
	std::vector<C3DTexture*>	_textures;

	osg::Vec4	_storedDiffuse;
	osg::Vec4	_storedSpecular;

	bool		_external = false;
};

class C3DTechnique : public C3DBaseTechnique
{
public:

	// Класс техники особо не детализируем. Предполагаем, что существует только одна техника.
	static C3DTechnique*	defaultTechnique();

	//@{ C3DBaseResource

	// дочерних ресурсов у техники нет.
	virtual void GetChildResources(std::vector<C3DBaseResource*>& out_vecChildResources) const {};

	// менеджер у техник отсутствует
	virtual C3DBaseManager*	GetManager() const { return nullptr; };

	//@} C3DBaseResource

};

//
//	C3DTexture
//

class C3DTexture : public C3DBaseTexture
{
public:

	C3DTexture(const std::string& fileName);

	//@{ C3DBaseTexture
	
	// вернуть тип текстуры
	virtual ETextureType	GetTextureType() const { return TEXTURE_2D; }

	//@} C3DBaseTexture


	//@{ C3DBaseResource

	// получить дочерние ресурсы. У текстур нет дочерних ресурсов.
	virtual void GetChildResources(std::vector<C3DBaseResource*>& out_vecChildResources) const {};

	// получить указатель на менеджер, управляющий данным ресурсом
	// может быть NULL. В таком случае, ресурс не является выгружаемым
	virtual C3DBaseManager*	GetManager() const;

	//@} C3DBaseResource


	// применить текстуру к узлу графа сцены. Вызывается при создании объектов
	osg::Texture2D*	getOsgTexture() const { return _texture; }

	// выгрузить текстуру. Эта функция вызывается из менеджера текстур.
	void unloadTexture();

	// загрузить текстуру. Эта функция вызывается из менеджера текстур.
	void loadTexture();
	
private:

	std::string						_fileName;
	osg::ref_ptr<osg::Texture2D>	_texture;
	osg::ref_ptr<osg::Image>		_image;
};
